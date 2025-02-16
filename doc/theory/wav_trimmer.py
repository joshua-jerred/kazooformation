from scipy.io import wavfile
from pydub import AudioSegment
from pydub.generators import WhiteNoise
from pydub.utils import ratio_to_db
import numpy as np
import os

def trim_wav(base_dir, input_file, start_sample_index, trim_length, sample_rate):
    # Convert to wav if needed
    created_wav = False
    if input_file.endswith(".mp3"):
        output_file = os.path.join(base_dir, input_file.replace(".mp3", ".wav"))
        audio = AudioSegment.from_mp3(base_dir + input_file)
        audio = audio.set_sample_width(2)
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
    output_file = f"{base}_sample{ext}"
    wavfile.write(output_file, sample_rate, trimmed_data)

    print(f"Trimmed file saved as {output_file}")

def add_noise(base_dir, output_dir, input_file, noise_level):
    assert noise_level <= 1.0 and noise_level >= 0.0

    input_file_path = os.path.join(base_dir, input_file)
    base, ext = os.path.splitext(input_file)
    output_file = f"{base}_noise_{noise_level}{ext}"
    output_path = output_dir + output_file

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

    print(f"Added noise to {output_path} - {noise_level}")