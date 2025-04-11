from scipy.io import wavfile
from pydub import AudioSegment
from pydub.generators import WhiteNoise
from pydub.utils import ratio_to_db
import numpy as np
import os

def trim_wav(input_file, output_file, start_sample_index, trim_length, sample_rate):
    # Convert to wav if needed
    created_wav = False
    if input_file.endswith(".mp3"):
        audio = AudioSegment.from_mp3(input_file)
        audio = audio.set_sample_width(2) # 16-bit PCM
        audio = audio.export(output_file, format="wav")
        input_file = output_file
        created_wav = True

    sample_rate, data = wavfile.read(input_file)

    if created_wav:
        os.remove(input_file)

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
    wavfile.write(output_file, sample_rate, trimmed_data)

    print(f"Trimmed file saved as {output_file}")

def add_noise(input_file, output_file, noise_level):
    assert noise_level <= 1.0 and noise_level >= 0.0


    # source https://stackoverflow.com/a/78926556/20310850
    audio = AudioSegment.from_file(input_file, format="wav")
    noise = WhiteNoise().to_audio_segment(duration=len(audio))
    adjusted_noise = noise.apply_gain(ratio_to_db(noise_level))
    audio = audio.overlay(adjusted_noise)

    # noise.export(output_dir + "white_noise.wav", format="wav")
    # sample_rate, data = wavfile.read(file_path)

    # if data.dtype != np.int16:
        # raise RuntimeError("not 16-bit PCM")

    audio.export(output_file, format="wav")

    # wavfile.write(output_path, sample_rate, data)

    print(f"Added noise to {output_file} - {noise_level}")