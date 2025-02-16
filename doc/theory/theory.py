import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile

FFT_MIN_FREQ = 0
FFT_MAX_FREQ = 20000
CARRIER_FREQ = 2500
CARRIER_FREQ_WIDTH = 500 # 500 Hz on either side of the carrier frequency
BASE_DIR = './doc/theory/'
input_files = ['KazooDoHigh_0.1s.wav', 'KazooDo_0.1s.wav']

def plot_fft(filename, axis):
    sample_rate, data = wavfile.read(BASE_DIR + filename)
    # Convert to mono by averaging channels if needed
    if len(data.shape) == 2:
        data = data.mean(axis=1)

    # Normalize 16-bit PCM data to the range [-1, 1]
    data = data / 2**15
    # Perform FFT
    fft_result = np.fft.fft(data)
    frequencies = np.fft.fftfreq(len(fft_result), 1 / sample_rate)
    # Take the positive half of the spectrum
    positive_freqs = frequencies[:len(frequencies)//2]
    positive_magnitudes = np.abs(fft_result[:len(fft_result)//2])

    # Remove samples above the maximum frequency
    positive_freqs = positive_freqs[positive_freqs < FFT_MAX_FREQ]
    positive_magnitudes = positive_magnitudes[:len(positive_freqs)]

    # Remove samples below the minimum frequency
    # positive_freqs = positive_freqs[positive_freqs > FFT_MIN_FREQ]
    # positive_magnitudes = positive_magnitudes[-len(positive_freqs):]

    # Calculate the average/mean frequency
    mean_freq = np.sum(positive_freqs * positive_magnitudes) / np.sum(positive_magnitudes)
    print(f'Mean frequency: {mean_freq:.2f} Hz')

    # Calculate the peak frequency
    peak_freq = positive_freqs[np.argmax(positive_magnitudes)]
    print(f'Peak frequency: {peak_freq:.2f} Hz')

    # Calculate if there is more energy above or below the carrier frequency
    carrier_index = np.argmin(np.abs(positive_freqs - CARRIER_FREQ))
    energy_below_carrier = np.sum(positive_magnitudes[:carrier_index])
    energy_above_carrier = np.sum(positive_magnitudes[carrier_index:])
    print(f'Energy above or below (negative if below): {energy_above_carrier - energy_below_carrier:.2f}')

    # Calculate the average frequency outside the carrier frequency
    outside_carrier_indices = np.where(
        (positive_freqs < CARRIER_FREQ - CARRIER_FREQ_WIDTH) |
        (positive_freqs > CARRIER_FREQ + CARRIER_FREQ_WIDTH))
    outside_carrier_freqs = positive_freqs[outside_carrier_indices]
    outside_carrier_magnitudes = positive_magnitudes[outside_carrier_indices]
    mean_outside_carrier_freq = np.sum(outside_carrier_freqs * outside_carrier_magnitudes) / np.sum(outside_carrier_magnitudes)
    print(f'Mean frequency outside carrier: {mean_outside_carrier_freq:.2f} Hz')

    # Plot the results
    axis.plot(positive_freqs, positive_magnitudes)

    axis.axvline(
        mean_freq,
        color = 'g',
        linestyle = '--',
        linewidth = 3)
    axis.text(
        mean_freq,
        50,
        f'Mean: {mean_freq:.2f} Hz',
        verticalalignment = 'top',
        horizontalalignment = 'center',
        color = 'g')

    axis.axvline(
        peak_freq,
        color = 'r',
        linestyle = '--',
        linewidth = 3)
    axis.text(
        peak_freq,
        60,
        f'Peak: {peak_freq:.2f} Hz',
        verticalalignment = 'top',
        horizontalalignment = 'center',
        color = 'r')

    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude')
    plt.grid()


def plot_all_files():
    figure, axis = plt.subplots(2, 2)
    figure.set_size_inches(10, 10)

    plot_number = 0
    for filename in input_files:
        plot_axis = axis[plot_number, 0]
        plot_axis.set_title(f'FFT of {filename}')
        plot_fft(filename, plot_axis)
        plot_number += 1

    plt.savefig(BASE_DIR + 'fft_results.png')

def main():
    plot_all_files()

if __name__ == '__main__':
    main()