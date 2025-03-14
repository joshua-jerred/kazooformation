import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from scipy.io import wavfile

import dsp

def plot_vertical_line(label, axis, x, color="black", linestyle="-", width=3, offset=0):
    axis.axvline(x, color=color, linestyle=linestyle, linewidth=width)
    axis.text(
        x,
        0.1,
        f"{label}: {x:.0f}",
        # verticalalignment="top",
        # horizontalalignment="center",
        color=color,
        rotation=60,
    )

def do_fft(base_dir, filename, fft_params, apply_filters):
    sample_rate, data = wavfile.read(base_dir + filename)

    if len(data.shape) != 1:
        raise RuntimeError("not mono")

    data = data / 2**15  # Normalize 16-bit PCM data to the range [-1, 1]

    # Apply notch and band pass filters to remove harmonics
    if apply_filters:
        print(f"Applying notch filter to {filename}")
        for notch_freq in fft_params["NOTCH_FREQS"]:
            data = dsp.notch_filter(
                data,
                sample_rate,
                notch_freq,
                fft_params["NOTCH_HARMONICS"],
                fft_params["NOTCH_QUALITY_FACTOR"],
            )
        for bandpass_freq in fft_params["BANDPASS_FREQS"]:
            data = dsp.bandpass_filter(
                data,
                sample_rate,
                bandpass_freq,
                fft_params["BANDPASS_BANDWIDTH"],
                fft_params["BANDPASS_ORDER"],
            )

    fft_result = np.fft.fft(data)
    frequencies = np.fft.fftfreq(len(fft_result), 1 / sample_rate)
    positive_freqs = frequencies[: len(frequencies) // 2]
    positive_magnitudes = np.abs(fft_result[: len(fft_result) // 2])

    # Remove samples above and below the maximum and minimum frequency
    positive_freqs = positive_freqs[positive_freqs < fft_params["FFT_MAX_FREQ"]]
    positive_magnitudes = positive_magnitudes[: len(positive_freqs)]
    positive_freqs = positive_freqs[positive_freqs > fft_params["FFT_MIN_FREQ"]]
    positive_magnitudes = positive_magnitudes[-len(positive_freqs) :]
    
    # Normalize the magnitudes
    positive_magnitudes = positive_magnitudes / np.max(positive_magnitudes)
    # positive_magnitudes = 1 - positive_magnitudes # Invert (st)

    if apply_filters:
        # normalize_factor = fft_params["NORMALIZE_FFT_FACTOR"]
        # positive_magnitudes = positive_magnitudes / normalize_factor

        gate_value = fft_params["NOISE_FLOOR_VALUE"]
        positive_magnitudes[positive_magnitudes < gate_value] = 0

    return positive_freqs, positive_magnitudes


def plot_fft(base_dir, filename, axis, fft_params: dict, apply_notch_filter):
    positive_freqs, positive_magnitudes = do_fft(
        base_dir, filename, fft_params, apply_notch_filter
    )

    # mean_freq = np.sum(positive_freqs * positive_magnitudes) / np.sum(positive_magnitudes)
    # print(f'Mean frequency: {mean_freq:.2f} Hz')
    # peak_freq = positive_freqs[np.argmax(positive_magnitudes)]
    # print(f'Peak frequency: {peak_freq:.2f} Hz')

    # Plot the 4 biggest peaks
    fundamental_freq_index_margin = fft_params["FUNDAMENTAL_FREQ_MARGIN_HZ"]
    max_fundamental_freqs = fft_params["MAX_FUNDEMENTAL_FREQS"]
    peak_indices = []
    for peak_index in np.argsort(positive_magnitudes)[::-1]:
        if len(peak_indices) >= max_fundamental_freqs:
            break # We have enough peaks

        within_margin = True
        for index_of_existing_peak in peak_indices:
            if abs(positive_freqs[peak_index] - positive_freqs[index_of_existing_peak]) < fundamental_freq_index_margin:
                within_margin = False
                break

        if within_margin:
            peak_indices.append(peak_index)

        peak_freq = positive_freqs[peak_index]

    # Plot the peaks
    i = 1
    for peak_index in peak_indices:
        peak_freq = positive_freqs[peak_index]
        plot_vertical_line(f"{i}", axis, peak_freq, color="r", width=1)
        i += 1
        
    # Detect fundamental frequency (first peak)
    peak_index = np.argmax(positive_magnitudes)
    # print(
    # f"Peak index: {peak_index}, Peak frequency: {positive_freqs[peak_index]:.2f} Hz"
    # )
    fundamental_freq = positive_freqs[peak_index]
    print(f"Fundamental frequency: {fundamental_freq:.2f} Hz")

    # Annotate detected fundamental frequency
    # axis.axvline(
    #     fundamental_freq,
    #     color="r",
    #     linestyle="dashed",
    #     label=f"Fundamental: {fundamental_freq:.2f} Hz",
    # )
    # axis.text(
    #     fundamental_freq,
    #     max(positive_magnitudes) * 0.8,
    #     f"{fundamental_freq:.2f} Hz",
    #     color="red",
    # )

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
    
    # ylim = max(positive_magnitudes) * 1.1
    # if apply_notch_filter:
        # ylim = 1
    # axis.set_ylim(0, ylim)

    # plot_vertical_line("Mean", axis, mean_outside_carrier_freq, color='g')
    # plot_vertical_line("Peak", axis, peak_freq, color='r')

    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude")
    plt.grid()


def plot_amplitude_distribution(
    base_dir, filename, axis, fft_params: dict, normalize, apply_notch_filter=False
):
    positive_freqs, positive_magnitudes = do_fft(
        base_dir, filename, fft_params, apply_notch_filter
    )

    normalized_magnitudes = positive_magnitudes / np.max(positive_magnitudes)
    variance = np.var(normalized_magnitudes)
    std_dev = np.std(normalized_magnitudes)
    mean = np.mean(normalized_magnitudes)
    median = np.median(normalized_magnitudes)
    print(f"Dist Variance: {variance:.4f}, Std Dev: {std_dev:.4f}, Mean: {mean:.4f}, Median: {median:.4f}")
    
    
    axis.hist(
        normalized_magnitudes,
        bins=fft_params["FFT_DISTRIBUTION_BINS"],
        edgecolor="black",
        alpha=0.7,
        range=(0, fft_params["FFT_DISTRIBUTION_MAX_NORMALIZED"]),
    )
    noise_floor = fft_params["NOISE_FLOOR_VALUE"]
    # axis.axvline(noise_floor, color="r", linestyle="--", label="Noise Floor")
    # axis.axvline(mean, color="g", linestyle="--", label="Mean")
    # axis.axvline(median, color="b", linestyle="--", label="Median")
    plot_vertical_line("Noise Floor", axis, noise_floor, color='r', linestyle='solid')
    plot_vertical_line("Mean", axis, mean, color='g', linestyle='--')
    plot_vertical_line("Median", axis, median, color='b', linestyle='--')
    plot_vertical_line("Std Dev", axis, mean + std_dev, color='orange', linestyle='--')

    plt.xlabel("Amplitude")
    plt.ylabel("Count")

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


def plot_files(base_dir, output_dir, input_files, fft_params):
    WAV_PLOT = True
    DIST_PLOT = True
    DIST_NORM_PLOT = True

    num_plots = 2
    num_plots += 1 if WAV_PLOT else 0
    num_plots += 1 if DIST_NORM_PLOT else 0

    figure, axis = plt.subplots(len(input_files), num_plots)

    plot_number = 0
    column_index = 0
    for filename in input_files:
        column_index = 0

        if WAV_PLOT:
            wav_axis = axis[plot_number, column_index]
            wav_axis.set_title(f"Waveform of {filename}")
            plot_wav_waveform(base_dir, filename, wav_axis, fft_params)
            column_index += 1

        fft_axis = axis[plot_number, column_index]
        fft_axis.set_title(f"Max Value Normalized FFT of {filename}")
        plot_fft(base_dir, filename, fft_axis, fft_params, False)
        column_index += 1

        fft_notch_axis = axis[plot_number, column_index]
        fft_notch_axis.set_title(f"FFT with Notch & Band Pass Filters of {filename}")
        plot_fft(base_dir, filename, fft_notch_axis, fft_params, True)
        column_index += 1

        if DIST_NORM_PLOT:
            norm_hist_axis = axis[plot_number, column_index]
            norm_hist_axis.set_title(f"Amplitude Distribution of {filename}")
            plot_amplitude_distribution(
                base_dir, filename, norm_hist_axis, fft_params, True
            )
            column_index += 1
        plot_number += 1

    figure.set_size_inches(column_index * 10, 5 * len(input_files))

    plt.savefig(output_dir + "fft_results.png", pad_inches=0.0)
