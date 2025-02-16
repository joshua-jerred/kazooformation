import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from scipy.io import wavfile


def plot_vertical_line(label, axis, x, color="black", linestyle="-", width=3):
    axis.axvline(x, color=color, linestyle=linestyle, linewidth=width)
    axis.text(
        x,
        80,
        f"{label}: {x:.0f} Hz",
        verticalalignment="top",
        horizontalalignment="center",
        color="black",
    )


def do_fft(base_dir, filename, fft_params):
    sample_rate, data = wavfile.read(base_dir + filename)

    if len(data.shape) != 1:
        raise RuntimeError("not mono")

    data = data / 2**15  # Normalize 16-bit PCM data to the range [-1, 1]
    fft_result = np.fft.fft(data)
    frequencies = np.fft.fftfreq(len(fft_result), 1 / sample_rate)
    positive_freqs = frequencies[: len(frequencies) // 2]
    positive_magnitudes = np.abs(fft_result[: len(fft_result) // 2])

    # Remove samples above and below the maximum and minimum frequency
    positive_freqs = positive_freqs[positive_freqs < fft_params["FFT_MAX_FREQ"]]
    positive_magnitudes = positive_magnitudes[: len(positive_freqs)]
    positive_freqs = positive_freqs[positive_freqs > fft_params["FFT_MIN_FREQ"]]
    positive_magnitudes = positive_magnitudes[-len(positive_freqs) :]

    return positive_freqs, positive_magnitudes


def plot_fft(base_dir, filename, axis, fft_params: dict):
    positive_freqs, positive_magnitudes = do_fft(base_dir, filename, fft_params)

    # mean_freq = np.sum(positive_freqs * positive_magnitudes) / np.sum(positive_magnitudes)
    # print(f'Mean frequency: {mean_freq:.2f} Hz')
    # peak_freq = positive_freqs[np.argmax(positive_magnitudes)]
    # print(f'Peak frequency: {peak_freq:.2f} Hz')

    # Calculate if there is more energy above or below the carrier frequency
    carrier_index = np.argmin(np.abs(positive_freqs - fft_params["CARRIER_FREQ"]))
    energy_below_carrier = np.sum(positive_magnitudes[:carrier_index])
    energy_above_carrier = np.sum(positive_magnitudes[carrier_index:])
    # print(f'Energy above or below (negative if below): {energy_above_carrier - energy_below_carrier:.2f}')

    # Calculate the average frequency outside the carrier frequency
    outside_carrier_indices = np.where(
        (positive_freqs < fft_params["CARRIER_FREQ"] - fft_params["CARRIER_FREQ_WIDTH"])
        | (
            positive_freqs
            > fft_params["CARRIER_FREQ"] + fft_params["CARRIER_FREQ_WIDTH"]
        )
    )
    outside_carrier_freqs = positive_freqs[outside_carrier_indices]
    outside_carrier_magnitudes = positive_magnitudes[outside_carrier_indices]
    mean_outside_carrier_freq = np.sum(
        outside_carrier_freqs * outside_carrier_magnitudes
    ) / np.sum(outside_carrier_magnitudes)
    # print(f'Mean frequency outside carrier: {mean_outside_carrier_freq:.2f} Hz')

    # Calculate which musical note the kazoo sound is closest to
    # positive_freqs
    # positive_magnitudes
    # Detect and annotate harmonics

    # Detect fundamental frequency (first peak)
    peak_index = np.argmax(positive_magnitudes)
    print(
        f"Peak index: {peak_index}, Peak frequency: {positive_freqs[peak_index]:.2f} Hz"
    )
    fundamental_freq = positive_freqs[peak_index]
    print(f"Fundamental frequency: {fundamental_freq:.2f} Hz")

    # Annotate detected fundamental frequency
    axis.axvline(
        fundamental_freq,
        color="r",
        linestyle="dashed",
        label=f"Fundamental: {fundamental_freq:.2f} Hz",
    )
    axis.text(
        fundamental_freq,
        max(positive_magnitudes) * 0.8,
        f"{fundamental_freq:.2f} Hz",
        color="red",
    )

    # harmonics = []
    # for i in range(2, 5):  # Checking up to the nth harmonic
    #     harmonic_freq = fundamental_freq * i
    #     idx = (np.abs(positive_freqs - harmonic_freq)).argmin()  # Find closest frequency bin
    #     if idx < len(positive_magnitudes):
    #         harmonics.append(harmonic_freq)
    #         axis.axvline(harmonic_freq, color='g', linestyle='dotted', alpha=0.7)
    #         axis.text(harmonic_freq, max(positive_magnitudes) * (0.7 - i * 0.05),
    #                  f"{harmonic_freq:.2f} Hz", color='green')

    axis.plot(positive_freqs, positive_magnitudes)

    # plot_vertical_line("Mean", axis, mean_outside_carrier_freq, color='g')
    # plot_vertical_line("Peak", axis, peak_freq, color='r')

    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude")
    plt.grid()


def plot_amplitude_distribution(base_dir, filename, axis, fft_params: dict, normalize):
    positive_freqs, positive_magnitudes = do_fft(base_dir, filename, fft_params)

    if normalize:
        normalized_magnitudes = positive_magnitudes / np.max(positive_magnitudes)
        axis.hist(
            normalized_magnitudes,
            bins=fft_params["FFT_DISTRIBUTION_BINS"],
            edgecolor="black",
            alpha=0.7,
        )
    else:
        axis.hist(
            positive_magnitudes,
            bins=fft_params["FFT_DISTRIBUTION_BINS"],
            edgecolor="black",
            alpha=0.7,
        )

    # axis.title(title)
    # axis.xlabel("Magnitude")
    # axis.ylabel("Count")
    plt.grid()


def plot_wav_waveform(base_dir, filename, axis, fft_params: dict):
    sample_rate, data = wavfile.read(base_dir + filename)
    # print(f"Data: {data}, sample rate: {sample_rate}")
    # Require 16-bit PCM data
    if data.dtype != np.int16:
        raise RuntimeError("not 16-bit PCM")
    axis.plot(data)
    axis.set_title(f"Waveform of {filename}")
    plt.xlabel("Sample")
    plt.ylabel("Amplitude")
    plt.grid()


def plot_files(base_dir, input_files, fft_params):
    NUM_PLOTS = 4
    figure, axis = plt.subplots(len(input_files), NUM_PLOTS)
    figure.set_size_inches(30, 15)

    plot_number = 0
    for filename in input_files:
        wav_axis = axis[plot_number, 0]
        wav_axis.set_title(f"Waveform of {filename}")
        plot_wav_waveform(base_dir, filename, wav_axis, fft_params)

        fft_axis = axis[plot_number, 1]
        fft_axis.set_title(f"FFT of {filename}")
        plot_fft(base_dir, filename, fft_axis, fft_params)

        hist_axis = axis[plot_number, 2]
        hist_axis.set_title(f"Amplitude Distribution of {filename}")
        plot_amplitude_distribution(base_dir, filename, hist_axis, fft_params, False)

        norm_hist_axis = axis[plot_number, 3]
        norm_hist_axis.set_title(f"Normalized Amplitude Distribution of {filename}")
        plot_amplitude_distribution(base_dir, filename, norm_hist_axis, fft_params, True)
        plot_number += 1

    plt.savefig(base_dir + "fft_results.png")
