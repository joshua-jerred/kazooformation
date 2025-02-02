"""Convert wav files to PCM data lists for use in C++ code."""

from scipy.io import wavfile

FILES_TO_CONVERT = [
    ("src/model_builder/k1_data/KazooDo_0.1s.wav",
     "src/ktl/models/k1_data/KazooDo_0.1s.pcm"),
    ("src/model_builder/k1_data/KazooDoHigh_0.1s.wav",
        "src/ktl/models/k1_data/KazooDoHigh_0.1s.pcm"),
]

def savePcmDataToFile(input_wav_path, output_path):
    sample_rate, data = wavfile.read(input_wav_path)

    with open(output_path, "w") as f:
        flattened = data.reshape(-1) # Handle multi-channel by flattening

        for sample in flattened:
            f.write(f"{sample},\n")

    print(f"Done. Samples saved to {output_path}")

if __name__ == "__main__":
    for wav_file, pcm_file in FILES_TO_CONVERT:
        savePcmDataToFile(wav_file, pcm_file)