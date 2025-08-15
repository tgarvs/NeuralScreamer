

"""
@author Thomas Garvey
@date August 11, 2025
@file model.py
@brief A RNN to model guitar distortion. The architecture for this model is given in https://dafx.de/paper-archive/2019/DAFx2019_paper_43.pdf
"""




import numpy as np
import tensorflow as tf
from tensorflow.keras import layers, optimizers
from tqdm.auto import tqdm
from RTNeural.python.model_utils import save_model
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.utils import shuffle

DATA_PATH = './audio/postproc/test-dataset.npz'

# --------------------------------------------------
# Data Loading (ensure shape (N, T, 1))
# --------------------------------------------------
def load_dataset(path):
    data = np.load(path)
    X = data['inputs'].astype('float32')
    y = data['targets'].astype('float32')

    #Uncomment to test truncated dataset during debugging
    # X = X[:5]
    # y = y[:5]

    print(f"Loaded Dataset Shape: X={X.shape}, Y={y.shape}")
    return X, y


def prepare_dataset(val_percentage=0.05):
    X, Y = load_dataset(DATA_PATH)
    X, Y = shuffle(X, Y, random_state=42) #sklearn.utils shuffle function keeps the pairs synced
    print(f"In prepare data, X shape : {X.shape}")

    #split into training/validation
    X_train, X_validation, Y_train, Y_validation = train_test_split(
        X, Y, test_size=val_percentage, shuffle=False)

    print(f"In prepare data, X_train shape AFTER traintestsplit : {X_train.shape}")
    print(f"In prepare data, X_val shape AFTER traintestsplit : {X_validation.shape}")
    
    return X_train, Y_train, X_validation, Y_validation



# --------------------------------------------------
# Loss Functions
# --------------------------------------------------

#High pass filter to boost high end --> good for high harmonic distortion
def pre_emphasis_filter(x, coeff=0.85):
  return tf.concat([x[:, 0:1, :], x[:, 1:, :] - coeff*x[:, :-1, :]], axis=1)

#DC offset
def dc_loss(target_y, predicted_y):
    return tf.reduce_mean(tf.square(tf.reduce_mean(target_y) - tf.reduce_mean(predicted_y))) / tf.reduce_mean(tf.square(target_y))

# Error-to-signal loss
def esr_loss(target_y, predicted_y, emphasis_func=lambda x : x):
    target_yp = emphasis_func(target_y)
    pred_yp = emphasis_func(predicted_y)
    return tf.reduce_sum(tf.square(target_yp - pred_yp)) / tf.reduce_sum(tf.square(target_yp))

#Final loss function put together
def combined_loss(y_true, y_pred):
    return esr_loss(y_true, y_pred) + dc_loss(y_true, y_pred)


# --------------------------------------------------
# Build Stateful Model
# --------------------------------------------------
def build_model(batch_size):
    tf.keras.backend.clear_session()
    m = tf.keras.Sequential([
        layers.Input(batch_shape=(batch_size, None, 2)), #two inputs used
        layers.LSTM(64, return_sequences=True, stateful=True, name='stateful_lstm'),
        layers.Dense(1, activation=None),
    ])
    return m

# --------------------------------------------------
# Build Stateless Inference Engine for Validation
# --------------------------------------------------
def build_inference_model():
    tf.keras.backend.clear_session()
    m = tf.keras.Sequential([
        layers.Input(shape=(None,2)), #two inputs used
        layers.LSTM(64, return_sequences=True, stateful=False, name='stateful_lstm'),
        layers.Dense(1, activation=None),
    ])
    return m

# --------------------------------------------------
# Training Loop Function
# --------------------------------------------------
def train_model(model, in_batches, out_batches, in_val, out_val, optimizer, epochs, warmup_len, segment_len, inf):

    #Lists to hold loss results for later graphing
    train_history = []
    val_history = []

    for ep in range(epochs):
        print(f"\n MiniBatch Epoch {ep+1}/{epochs}")
        batch_losses = []

        #shuffle the training set at the start of each epoch
        in_batches, out_batches = shuffle(in_batches, out_batches)

        for xb, yb in zip(in_batches, out_batches): #cycles through the batches until the entire dataset is processed
            # print(f"shape of xb in train : {xb.shape}")
            model.get_layer('stateful_lstm').reset_states() #reset between each batch
            model(xb[:, :warmup_len, :]) #warm up before each batch

            #calculate loss per sequence
            loss = step_learn(model, xb, yb, optimizer, warmup_len, segment_len)
            batch_losses.append(loss.numpy()) #append all the sequence losses in one list
        
        #average loss across the epoch
        avg_train = float(np.mean(batch_losses))
        train_history.append(avg_train)
        print(f" Avg Train Loss: {avg_train:.6f}")


        if ep % 2 == 0:
            # print(f"Shape of validation dataset per epoch: {in_val.shape}")
    
            inf.get_layer('stateful_lstm').reset_states()
            inf.set_weights(model.get_weights())
            y_pred_val = inf.predict(in_val, batch_size=32)
            # print("VAL PREDICTED")
            val_loss = combined_loss(out_val, y_pred_val).numpy()
            val_history.append(val_loss)
            print(f"  Val Loss: {val_loss:.6f}")

    return train_history, val_history


# --------------------------------------------------
# Single‐step train (stateful). Separated out into a tf.function to run faster...not exactly sure how this works here...
# --------------------------------------------------
@tf.function
def step_learn(model, x_seq, y_seq, optimizer, warmup, seglen):
    total_loss = tf.constant(0.0)
    count = tf.constant(0)

    for n in tf.range(warmup, tf.shape(x_seq)[1] - seglen, seglen):
        target = y_seq[:, n:n+seglen, :]

        with tf.GradientTape() as tape:
            pred = model(x_seq[:, n:n+seglen, :])
            loss = combined_loss(target, pred)

        grads = tape.gradient(loss, model.trainable_variables)
        grads, _ = tf.clip_by_global_norm(grads, 1.0) #clip gradient

        optimizer.apply_gradients(zip(grads, model.trainable_variables))
        total_loss += loss
        count += 1

    #returns average loss across all the batch
    return total_loss / tf.cast(tf.maximum(count, 1), tf.float32)


# --------------------------------------------------
# Plot Waveforms
# --------------------------------------------------
def check_waveform(model, X, y, label):
    for i in range(3):
        x_vis = X[i:i+1]
        y_vis = y[i].squeeze()
        y_pred = model.predict(x_vis, verbose=0).squeeze()
        plt.figure(figsize=(14,5))
        plt.plot(y_vis, label='Target', alpha=0.75)
        plt.plot(y_pred, label='Prediction', alpha=0.75)
        plt.title(label)
        plt.xlabel("Time (samples)")
        plt.ylabel("Amplitude")
        plt.legend()
        plt.grid(True)
        plt.show()

# --------------------------------------------------
# Main
# --------------------------------------------------
if __name__ == '__main__':
    # Load & split
    X_train, y_train, X_val, y_val = prepare_dataset()

    # Parameters
    BATCH_SIZE = 500
    warmup_len = 1000
    segment_len = 2048
    epochs = 500

    # Prepare train batches so theyre the exact shape with no wrong sized batches left over
    NUM_TRAIN, NUM_SAMPLES, _ = X_train.shape
    n_batches = NUM_TRAIN // BATCH_SIZE

    X_trunc = X_train[:n_batches * BATCH_SIZE, :, :]
    y_trunc = y_train[:n_batches * BATCH_SIZE, :, :]
    in_batches  = X_trunc.reshape((n_batches, BATCH_SIZE, NUM_SAMPLES, 2)) #i.e. 2 batches of 40, each with 22050 samples and 1 feature 
    out_batches = y_trunc.reshape((n_batches, BATCH_SIZE, NUM_SAMPLES, 1))

    # Prepare validation data
    IN_val  = X_val # .reshape((X_val.shape[0], NUM_SAMPLES, 2))
    OUT_val = y_val # .reshape((y_val.shape[0], NUM_SAMPLES, 1))


    # Build model and optimizer
    model = build_model(batch_size=BATCH_SIZE)
    inf = build_inference_model()
    optimizer = optimizers.Adam(5e-4)

    # Train
    train_hist, val_hist = train_model(model, in_batches, out_batches, IN_val, OUT_val, optimizer, epochs, warmup_len, segment_len, inf)

    # Save model
    save_model(model, './model_export/ts_nine.json')


    # Plot training history
    plt.plot(train_hist, label='Train Loss')
    plt.plot(list(range(0, epochs, 2)), val_hist, label='Val Loss')
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.legend()
    plt.grid(True)
    plt.show()

    # Inference waveform checks
    infer = build_inference_model()
    infer.set_weights(model.get_weights())
    check_waveform(infer, X_val,   y_val,   "Validation Waveforms")