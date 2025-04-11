# Run from the project root directory

import wav_trimmer
import fft_plotter

SAMPLE_RATE = 44100
BASE_DIR = "./doc/k3_model/"
OUTPUT_DIR = "./doc/k3_model/output/"

# FILES_TO_CONVERT = ["kazooA.mp3", "kazooD.mp3", "kazooE.mp3", "kazooG.mp3"]
FILES = ["doot_0", "doot_1", "doot_2", "doot_3"]
# FILES_TO_CONVERT = [f"{file}.mp3" for file in FILES]

# Where
START_OFFSET_SECONDS = 0
START_OFFSET_SAMPLES = 0
SAMPLE_START_POS = int(SAMPLE_RATE * START_OFFSET_SECONDS) + START_OFFSET_SAMPLES
# What
SAMPLE_LENGTH_SECONDS = 0.1
SAMPLE_LENGTH = int(SAMPLE_RATE * SAMPLE_LENGTH_SECONDS)
print(f"Baud rate: {SAMPLE_RATE / SAMPLE_LENGTH}")

for file in FILES:
    # Convert to wav and trim
    # input_path = BASE_DIR + file + ".mp3"
    input_path = BASE_DIR + file + ".wav"
    output_path = OUTPUT_DIR + file + ".wav"
    wav_trimmer.trim_wav(input_path, output_path, SAMPLE_START_POS, SAMPLE_LENGTH, SAMPLE_RATE)

    # Next up, split the sample files into 4 parts, then plot the FFT of each part
    NUMBER_OF_PARTS = 4
    QUARTER_SAMPLE_LENGTH = int(SAMPLE_LENGTH / NUMBER_OF_PARTS)
    # for i in range(NUMBER_OF_PARTS):
        # wav_trimmer.trim_wav(output_path, OUTPUT_DIR + f"{file}_{i}.wav", i * QUARTER_SAMPLE_LENGTH, QUARTER_SAMPLE_LENGTH, SAMPLE_RATE)

# wav_trimmer.add_noise(OUTPUT_DIR + "kazooA_0.wav", OUTPUT_DIR + "kazooA_0_noise_0.1.wav", 0.1)
# wav_trimmer.add_noise(OUTPUT_DIR + "kazooA_0.wav", OUTPUT_DIR + "kazooA_0_noise_0.2.wav", 0.2)
# wav_trimmer.add_noise(OUTPUT_DIR + "kazooA_0.wav", OUTPUT_DIR + "kazooA_0_noise_0.4.wav", 0.4)
# wav_trimmer.add_noise(OUTPUT_DIR + "kazooA_0.wav", OUTPUT_DIR + "kazooA_0_noise_0.6.wav", 0.6)
# wav_trimmer.add_noise(OUTPUT_DIR + "kazooA_0.wav", OUTPUT_DIR + "kazooA_0_noise_0.8.wav", 0.8)
# wav_trimmer.add_noise(OUTPUT_DIR + "kazooA_0.wav", OUTPUT_DIR + "kazooA_0_noise_0.95.wav", 0.95)


FFT_INPUT_FILES = [
    # "doot_0.wav",
    "doot_1.wav",
    "doot_2.wav",
    # "doot_3.wav",
    # "doot_1_0.wav",
    # "kazooA_0.wav",
    # "kazooA_0_noise_0.1.wav",
    # "kazooA_0_noise_0.2.wav",
    # "kazooA_0_noise_0.4.wav",
    # "kazooA_0_noise_0.6.wav",
    # "kazooA_0_noise_0.8.wav",
    # "kazooA_0_noise_0.95.wav",
    # "kazooA_1.wav",
    # "kazooA_2.wav",
    # "kazooA_3.wav",
    # "kazooD_0.wav",
    # "kazooD_1.wav",
    # "kazooD_2.wav",
    # "kazooD_3.wav",
    # "kazooE_0.wav",
    # "kazooE_1.wav",
    # "kazooE_2.wav",
    # "kazooE_3.wav",
    # "kazooG_0.wav",
    # "kazooG_1.wav",
    # "kazooG_2.wav",
    # "kazooG_3.wav",
]

fft_params = {
    "FFT_MIN_FREQ": 1250,
    "FFT_MAX_FREQ": 4000,
    "CARRIER_FREQ": 2000,
    "CARRIER_FREQ_WIDTH": 100,  # 500 Hz on either side of the carrier frequency
    "FFT_DISTRIBUTION_BINS": 20,
    "FFT_DISTRIBUTION_MAX": 50,
    "FFT_DISTRIBUTION_MAX_NORMALIZED": 1.0,
    # "NOTCH_FREQS": [1100, 1700, 3080],
    "NOTCH_FREQS": [
        # 1200
        ],
    "NOTCH_HARMONICS": 5,
    "NOTCH_QUALITY_FACTOR": 30,
    # "BANDPASS_FREQS": [2500],
    "BANDPASS_FREQS": [
        # 1400,
        # 3000
        ],
    "BANDPASS_BANDWIDTH": 200,
    "BANDPASS_ORDER": 5,
    "NORMALIZE_FFT_FACTOR": 30,
    "NOISE_FLOOR_VALUE": 0.3,
    "FUNDAMENTAL_FREQ_MARGIN_HZ": 100,
    "MAX_FUNDEMENTAL_FREQS": 2,
}
fft_plotter.plot_files(OUTPUT_DIR, OUTPUT_DIR, FFT_INPUT_FILES, fft_params)
