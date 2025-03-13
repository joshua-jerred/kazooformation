# Run from the project root directory

import wav_trimmer
import fft_ploter

SAMPLE_RATE = 44100
BASE_DIR = "./doc/theory/"
OUTPUT_DIR = "./doc/theory/output/"

FILES_TO_CONVERT = ["kazooA.mp3", "kazooD.mp3", "kazooE.mp3", "kazooG.mp3"]

# Where
START_OFFSET_SECONDS = 1
START_OFFSET_SAMPLES = 100
SAMPLE_START_POS = int(SAMPLE_RATE * START_OFFSET_SECONDS) + START_OFFSET_SAMPLES
# What
SAMPLE_LENGTH_SECONDS = 0.1
SAMPLE_LENGTH = int(SAMPLE_RATE * SAMPLE_LENGTH_SECONDS)
# SAMPLE_LENGTH = 1000
print(f"Baud rate: {SAMPLE_RATE / SAMPLE_LENGTH}")

for file in FILES_TO_CONVERT:
    wav_trimmer.trim_wav(BASE_DIR, file, SAMPLE_START_POS, SAMPLE_LENGTH, SAMPLE_RATE)

NOISE_FILES = [
    ("kazooA_sample.wav", 0.2),
    # ("kazooA_sample.wav", 0.4),
    # ("kazooA_sample.wav", 0.6),
    # ("kazooA_sample.wav", 0.8),
]
for file, noise_level in NOISE_FILES:
    wav_trimmer.add_noise(BASE_DIR, OUTPUT_DIR, file, noise_level)

FFT_INPUT_FILES = []
FFT_INPUT_FILES = [
    # "kazooE_sample.wav",
    "kazooA_sample.wav",
    # "output/kazooA_sample_noise_0.2.wav",
    # f"output/kazooA_sample_noise_0.4.wav",
    # f"output/kazooA_sample_noise_0.6.wav",
    # f"output/kazooA_sample_noise_0.8.wav",
    # "kazooG_sample.wav",
    "kazooD_sample.wav",
    "white_noise.wav",
]

# FFT_INPUT_FILES = ["KazooDoHigh_0.1s.wav", "KazooDo_0.1s.wav"]
fft_params = {
    "FFT_MIN_FREQ": 1000,
    "FFT_MAX_FREQ": 3000,
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
    "NORMALIZE_FFT_FACTOR": 30,
    "NOISE_FLOOR_VALUE": 0.3,
    "FUNDAMENTAL_FREQ_MARGIN_HZ": 100,
    "MAX_FUNDEMENTAL_FREQS": 4,
}
fft_ploter.plot_files(BASE_DIR, OUTPUT_DIR, FFT_INPUT_FILES, fft_params)
