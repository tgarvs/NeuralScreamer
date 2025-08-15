
"""
PREPROCESSING FOR TWO INPUTS (I.E. ONE KNOB)
"""




import os
import librosa
import numpy as np
import soundfile as sf
from sklearn.utils import shuffle
import matplotlib.pyplot as plt


PATH_TO_AUDIO = "./audio"
SAMPLE_RATE = 44100
CHUNK_SIZE = SAMPLE_RATE // 2  #0.5 seconds
HOP_SIZE = CHUNK_SIZE  # non-overlapping
SAVE_PATH = "./audio/postproc/test-dataset.npz"
LOAD_SUBDIR = "preproc"
EXPORT_DIR_INPUTS = "./audio/chunks/inputs"
EXPORT_DIR_TARGETS = "./audio/chunks/targets"
MAX_MISALIGNMENT_SAMPLES = 55  # Maximum allowed misalignment in samples


os.makedirs(EXPORT_DIR_INPUTS, exist_ok=True)
os.makedirs(EXPORT_DIR_TARGETS, exist_ok=True)
os.makedirs(os.path.dirname(SAVE_PATH), exist_ok=True)



# ─── COLLECT FILES ────────────────────────────────────────────────────────
input_files = {}
target_files = {}
for root, _, files in os.walk(PATH_TO_AUDIO):
    if os.path.basename(root) == LOAD_SUBDIR:
        for fname in files:
            if fname.endswith("-input.wav"):
                input_files[fname] = os.path.join(root, fname)
            elif fname.endswith("-target.wav"):
                target_files[fname] = os.path.join(root, fname)



# ─── PROCESS ───────────────────────────────────────────────────────────────
inputs = []
targets = []
chunk_idx = 0

#Cycle through all file names
for in_fname, in_path in sorted(input_files.items()):

    #Search for target that corresponds with each input
    tgt_fname = in_fname.replace("-input.wav", "-target.wav")
    print(f"Processing {in_fname} and {tgt_fname}")
    if tgt_fname not in target_files:
        print(f"Missing target for: {in_fname}")
        continue

    # Extract audio signal
    try:
        x, _ = librosa.load(in_path, sr=SAMPLE_RATE, mono=True)
        y, _ = librosa.load(target_files[tgt_fname], sr=SAMPLE_RATE, mono=True)
    except Exception as e:
        print(f"Error loading {in_fname}: {e}")
        continue


    # Extract knob position
    knob_position = in_fname.split("-")[1]

    # Trim signals so chunks are the perfect length
    L = min(len(x), len(y))
    if L < CHUNK_SIZE:
        continue

    x = x[:L]
    y = y[:L]

    # Normalize globally
    peak = max(np.max(np.abs(x)), np.max(np.abs(y)))
    x /= peak
    y /= peak


    #PLOT1: Checking the overall waveforms of both original big signals
    plt.figure(figsize=(14, 5))
    plt.plot(x, label='Input', alpha=0.50)
    plt.plot(y, label='Target', alpha=0.50)
    plt.title("Input vs Target Waveform")
    plt.xlabel("Time (samples)")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


    # Chunk
    for start in range(0, L - CHUNK_SIZE + 1, HOP_SIZE):
        x_chunk = x[start:start + CHUNK_SIZE]
        y_chunk = y[start:start + CHUNK_SIZE]

        # ─── MISALIGNMENT CHECK ──────────────────────────────────────────
        corr = np.correlate(x_chunk, y_chunk, mode='full')
        lag = np.argmax(corr) - (len(x_chunk) - 1)
        if abs(lag) > MAX_MISALIGNMENT_SAMPLES:
            print(f"Skipped chunk at sample {start} due to misalignment (lag={lag} samples)")
            continue  # Skip misaligned chunk

        #Probably don't need to export the 0.5 seconds chunks, but still kinda nice to hear them
        chunk_idx += 1
        sf.write(f"{EXPORT_DIR_INPUTS}/input_{chunk_idx:05d}.wav", x_chunk, SAMPLE_RATE)
        sf.write(f"{EXPORT_DIR_TARGETS}/target_{chunk_idx:05d}.wav", y_chunk, SAMPLE_RATE)

        kn = np.full(CHUNK_SIZE, knob_position) #make another array the same size as the chunk but filled with knob pos
        x_chunk = np.stack([x_chunk, kn], axis=-1) #stack knob array and x_chunk so that each 0.5 second chunk has its own personal knob position


        #Append each 0.5 second chunk to a bigger list
        inputs.append(x_chunk)
        targets.append(y_chunk)



if not inputs:
    print("No valid chunks found.")
    exit(1)

# ─── SAVE ─────────────────────────────────────────────────────────────────
X = np.array(inputs, dtype=np.float32)#[..., np.newaxis]
Y = np.array(targets, dtype=np.float32)[..., np.newaxis]


print(f"Final dataset: X={X.shape}, Y={Y.shape}")
np.savez(SAVE_PATH, inputs=X, targets=Y)
print(f"Saved dataset to: {SAVE_PATH}")






