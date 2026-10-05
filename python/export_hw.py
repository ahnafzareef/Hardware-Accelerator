import os
import numpy as np

CHUNK   = 64
OUT_DIR = "weights/hw"
IDX     = 7;      


def n_chunks(n_in):
    return (n_in + CHUNK - 1) // CHUNK            


def bits(vec):
    """+1/-1 list -> '1'/'0' string, item 0 on the right."""
    return "".join("1" if v == 1 else "0" for v in vec[::-1])


def write_weights(W, path):
    n_in, n_out = W.shape
    chunks = n_chunks(n_in)
    padded = np.ones((chunks * CHUNK, n_out), dtype=np.int8)   
    padded[:n_in] = W
    with open(path, "w") as f:
        for neuron in range(n_out):
            for c in range(chunks):
                f.write(bits(padded[c * CHUNK:(c + 1) * CHUNK, neuron]) + "\n")
    print(f"wrote {path}: {n_out * chunks} rows")


def write_thresholds(T, n_in, path):
    t = np.clip(np.ceil(T), 0, n_in + 1).astype(int)   
    with open(path, "w") as f:
        for v in t:
            f.write(f"{v:x}\n")
    print(f"wrote {path}: {len(t)} thresholds")


def write_test_image(path):
    os.environ["TF_USE_LEGACY_KERAS"] = "0"
    os.environ.setdefault("TF_CPP_MIN_LOG_LEVEL", "2")
    import keras
    (_, _), (imgs, labels) = keras.datasets.mnist.load_data()
    img = (imgs[IDX] / 127.5) - 1.0
    x = np.where(img.flatten() >= 0, 1, -1)            
    with open(path, "w") as f:
        f.write(bits(x) + "\n")
    print(f"wrote {path}: picture {IDX}, it's a {labels[IDX]}") #check number


if __name__ == "__main__":
    os.makedirs(OUT_DIR, exist_ok=True)
    write_weights(np.load("weights/layer1_weights.npy"), f"{OUT_DIR}/layer1_weights.mem")
    write_weights(np.load("weights/layer2_weights.npy"), f"{OUT_DIR}/layer2_weights.mem")
    write_weights(np.load("weights/layer3_weights.npy"), f"{OUT_DIR}/layer3_weights.mem")
    write_thresholds(np.load("weights/layer1_threshold.npy"), 784, f"{OUT_DIR}/layer1_thresh.mem")
    write_thresholds(np.load("weights/layer2_threshold.npy"), 256, f"{OUT_DIR}/layer2_thresh.mem")
    write_test_image(f"{OUT_DIR}/test_image.mem")