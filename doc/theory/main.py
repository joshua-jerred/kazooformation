import wav_trimmer
import fft_ploter

BASE_DIR = "./doc/theory/"

FILES_TO_CONVERT = ["kazooA.mp3", "kazooD.mp3", "kazooE.mp3", "kazooG.mp3"]
SAMPLE_RATE = 44100
START_OFFSET_SECONDS = 1
SAMPLE_LENGTH_SECONDS = 0.1
SAMPLE_START_POS = int(SAMPLE_RATE * START_OFFSET_SECONDS)
SAMPLE_LENGTH = int(SAMPLE_RATE * SAMPLE_LENGTH_SECONDS)  # * seconds
for file in FILES_TO_CONVERT:
    wav_trimmer.trim_wav(BASE_DIR, file, SAMPLE_START_POS, SAMPLE_LENGTH, SAMPLE_RATE)


# FFT_INPUT_FILES = []
FFT_INPUT_FILES = ["kazooA_sample.wav", "kazooD_sample.wav", "kazooE_sample.wav", "kazooG_sample.wav"]
# FFT_INPUT_FILES = ["KazooDoHigh_0.1s.wav", "KazooDo_0.1s.wav"]
fft_params = {
    "FFT_MIN_FREQ": 1000,
    "FFT_MAX_FREQ": 10000,
    "CARRIER_FREQ": 2500,
    "CARRIER_FREQ_WIDTH": 500,  # 500 Hz on either side of the carrier frequency
    "FFT_DISTRIBUTION_BINS": 100,
    # "FFT_DISTRIBUTION_MIN": 
}
fft_ploter.plot_files(BASE_DIR, FFT_INPUT_FILES, fft_params)
