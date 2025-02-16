from scipy.io import wavfile
from pydub import AudioSegment

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