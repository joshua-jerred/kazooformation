import scipy.signal as signal

def notch_filter(signal_data, sample_rate, fundamental_freq, harmonics, quality_factor):
    filtered_signal = signal_data

    for i in range(1, harmonics + 1):
        notch_freq = fundamental_freq * i  # Compute harmonic frequency
        if notch_freq >= sample_rate / 2:  # Avoid filtering above Nyquist frequency
            break
        b, a = signal.iirnotch(
            notch_freq, quality_factor, sample_rate
        )  # Notch filter coefficients
        filtered_signal = signal.filtfilt(b, a, filtered_signal)  # Apply filter

    return filtered_signal

def bandpass_filter(signal_data, sample_rate, center_freq, bandwidth=50, order=4):
    low_cutoff = max(center_freq - bandwidth / 2, 0)
    high_cutoff = min(center_freq + bandwidth / 2, sample_rate / 2)

    b, a = signal.butter(order, [low_cutoff, high_cutoff], btype="band", fs=sample_rate)
    filtered_signal = signal.filtfilt(b, a, signal_data)

    return filtered_signal
