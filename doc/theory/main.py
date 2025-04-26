# Run from the project root directory

import time
import os
import sys

import wav_trimmer
import fft_ploter

SAMPLE_RATE = 44100
# BASE_DIR = "./doc/theory/"
INPUT_DIR = "./doc/theory/input_audio/"
GENERATED_DIR = "./doc/theory/input_audio/generated/"
OUTPUT_DIR = "./doc/theory/output/"

# Delete all files in the output and generated directories
for dir in [OUTPUT_DIR, GENERATED_DIR]:
    files = os.listdir(dir)
    for file in files:
        file_path = os.path.join(dir, file)
        try:
            if os.path.isfile(file_path):
                os.remove(file_path)
                print(f"Deleted {file_path}")
            else:
                print(f"{file_path} is not a file")
        except Exception as e:
            print(f"Error deleting {file_path}: {e}")


# Convert files from .mp3 to .wav
# FILES_TO_CONVERT = ["kazooA.mp3", "kazooD.mp3", "kazooE.mp3", "kazooG.mp3"]

# FFT parameters
FFT_INPUT_FILE_PACKS = [
    # Basic waveforms
    # {
    #     "label": "Basic Waveforms",
    #     "trim_files":[], # Files to trim
    #     "noise_files":[], # Files to add noise to
    #     "fft_files":[ # Files to plot
    #         "white_noise.wav",
    #         "ex1_perfect.wav",
    #         "ex1_doot_messy.wav",
    #         "ex1_doot_clean.wav",
    # ]},

    # Kazoo Do
    # {
    #     "label": "KazooDo 0.1s",
    #     "trim_files":[],
    #     "noise_files":[
    #         ("KazooDo_0.1s.wav", 0.2),
    #         ("KazooDo_0.1s.wav", 0.4),
    #         ("KazooDo_0.1s.wav", 0.6),
    #         ("KazooDo_0.1s.wav", 0.8),
    #     ],
    #     "fft_files":[
    #         "KazooDo_0.1s.wav",
    #         "generated/KazooDo_0.1s_noise_0.2.wav",
    #         "generated/KazooDo_0.1s_noise_0.4.wav",
    #         "generated/KazooDo_0.1s_noise_0.6.wav",
    #         "generated/KazooDo_0.1s_noise_0.8.wav",
    # ]},

    # K3 Model Doot 0
    {
        "label": "K3 Model Doot 0",
        "trim_files":[
            {
                "input": "K3_model_doot_0.wav",
                "start_sample_index": 0,
                "length_in_samples": 4410,
                "itterations": 4
            },
        ],
        "noise_files":[],
        "fft_files":[
            "K3_model_doot_0.wav",
            "generated/K3_model_doot_0_sample.wav",
            "generated/K3_model_doot_0_sample.wav",
            "generated/K3_model_doot_0_sample.wav",
            "generated/K3_model_doot_0_sample.wav",
    ]},

    # "kazooE_sample.wav",
    # "kazooA_sample.wav",
    # "KazooDo_0.1s.wav",
    # "KazooDoHigh_0.1s.wav",
    # "output/kazooA_sample_noise_0.2.wav",
    # f"output/kazooA_sample_noise_0.4.wav",
    # f"output/kazooA_sample_noise_0.6.wav",
    # f"output/kazooA_sample_noise_0.8.wav",
    # "kazooG_sample.wav",
    # "kazooD_sample.wav",
]
FFT_PARAMS = {
    "FFT_MIN_FREQ": 500,
    "FFT_MAX_FREQ": 5000,
    "CARRIER_FREQ": 2000,
    "CARRIER_FREQ_WIDTH": 100,  # 500 Hz on either side of the carrier frequency
    "FFT_DISTRIBUTION_BINS": 100,
    "FFT_DISTRIBUTION_MAX": 50,
    "FFT_DISTRIBUTION_MAX_NORMALIZED": 0.5,
    "NOTCH_FREQS": [1100, 1700, 3080],
    "NOTCH_HARMONICS": 5,
    "NOTCH_QUALITY_FACTOR": 30,
    # "BANDPASS_FREQS": [2500],
    "BANDPASS_FREQS": [],
    "BANDPASS_BANDWIDTH": 200,
    "BANDPASS_ORDER": 5,
    # "NORMALIZE_FFT_FACTOR": 30,
    "NOISE_FLOOR_VALUE": 0.3,
    "FUNDAMENTAL_FREQ_MARGIN_HZ": 100,
    "MAX_FUNDEMENTAL_FREQS": 4,
}

# Where
START_OFFSET_SECONDS = 1
START_OFFSET_SAMPLES = 100
SAMPLE_START_POS = int(SAMPLE_RATE * START_OFFSET_SECONDS) + START_OFFSET_SAMPLES
# What
SAMPLE_LENGTH_SECONDS = 0.1
SAMPLE_LENGTH = int(SAMPLE_RATE * SAMPLE_LENGTH_SECONDS)
# SAMPLE_LENGTH = 1000
print(f"Baud rate: {SAMPLE_RATE / SAMPLE_LENGTH}")

# for file in FILES_TO_CONVERT:
    # wav_trimmer.trim_wav(INPUT_DIR, file, SAMPLE_START_POS, SAMPLE_LENGTH, SAMPLE_RATE)

# for file, noise_level in NOISE_FILES:
    # wav_trimmer.add_noise(INPUT_DIR, OUTPUT_DIR, file, noise_level)

for pack in FFT_INPUT_FILE_PACKS:
    label = pack["label"]
    fft_files = pack["fft_files"]

    start_time = time.time()
    print(f">> Processing {label} files")

    for file in pack["trim_files"]:
        input_file = file["input"]
        start_sample_index = file["start_sample_index"]
        length_in_samples = file["length_in_samples"]
        iterations = 1
        if "itterations" in file:
            iterations = file["itterations"]

        for i in range(iterations):
            output_file = f"{input_file}_trimmed_{i}.wav"
            wav_trimmer.trim_wav(
                INPUT_DIR + file["input"],
                GENERATED_DIR + output_file,
                file["start_sample_index"],
                file["length_in_samples"]
            )

    for file, noise_level in pack["noise_files"]:
        wav_trimmer.add_noise(INPUT_DIR, GENERATED_DIR, file, noise_level)

    fft_ploter.plot_files(INPUT_DIR, OUTPUT_DIR, label, fft_files, FFT_PARAMS)

    elapsed_time = time.time() - start_time
    print(f"  >> Finished processing in {elapsed_time:.2f} seconds")
    # print(f"Processing {name} files")
    # for file in files:
    #     wav_trimmer.trim_wav(INPUT_DIR, file, SAMPLE_START_POS, SAMPLE_LENGTH, SAMPLE_RATE)
    #     wav_trimmer.add_noise(INPUT_DIR, OUTPUT_DIR, file, 0.2)
