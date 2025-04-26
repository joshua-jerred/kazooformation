from scipy.io import wavfile
from pydub import AudioSegment
from pydub.generators import WhiteNoise
from pydub.utils import ratio_to_db
import numpy as np
import os

def convert_to_wav(input_file, output_file):
    audio = AudioSegment.from_mp3(input_file)
    audio = audio.set_sample_width(2)
    audio = audio.export(output_file, format="wav")

def trim_wav(input_file, output_file, start_sample_index, trim_length, sample_rate=44100):
    print(f"  >> Trimming {input_file} to {output_file} - {start_sample_index}, {trim_length} ... ", end="")

    __sample_rate, data = wavfile.read(input_file)

    if data.dtype != np.int16:
        raise RuntimeError("not 16-bit PCM")
    # if len(data.shape) == 2:
        # data = data.mean(axis=1)

    if start_sample_index < 0 or start_sample_index >= len(data):
        raise ValueError(f"start_sample_index invalid: {start_sample_index} not in [0, {len(data)})")
    if trim_length <= 0 or start_sample_index + trim_length > len(data):
        raise ValueError(f"trim_length invalid: {trim_length} not in (0, {len(data) - start_sample_index}]")

    trimmed_data = data[start_sample_index:start_sample_index+trim_length]
    base, ext = os.path.splitext(input_file)
    # output_file = f"{base}_sample{ext}"
    wavfile.write(output_file, sample_rate, trimmed_data)
    print("done")

def add_noise(base_dir, output_dir, input_file, noise_level):
    assert noise_level <= 1.0 and noise_level >= 0.0

    input_file_path = os.path.join(base_dir, input_file)
    base, ext = os.path.splitext(input_file)
    output_file = f"{base}_noise_{noise_level}{ext}"
    output_path = output_dir + output_file
    print(f"  >> Adding noise to {output_path} - {noise_level}")

    # source https://stackoverflow.com/a/78926556/20310850
    audio = AudioSegment.from_file(input_file_path, format="wav")
    noise = WhiteNoise().to_audio_segment(duration=len(audio))
    adjusted_noise = noise.apply_gain(ratio_to_db(noise_level))
    audio = audio.overlay(adjusted_noise)
    
    # noise.export(output_dir + "white_noise.wav", format="wav")
    # sample_rate, data = wavfile.read(file_path)

    # if data.dtype != np.int16:
        # raise RuntimeError("not 16-bit PCM")

    audio.export(output_path, format="wav")

    # wavfile.write(output_path, sample_rate, data)
