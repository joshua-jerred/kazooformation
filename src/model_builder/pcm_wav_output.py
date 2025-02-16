"""Convert wav files to PCM data lists for use in C++ code."""

from scipy.io import wavfile

FILES_TO_CONVERT = [
    (
        "src/model_builder/k1_data/KazooDo_0.1s.wav",
        "src/ktl/models/k1_data/KazooDo_0.1s.pcm",
    ),
    (
        "src/model_builder/k1_data/KazooDoHigh_0.1s.wav",
        "src/ktl/models/k1_data/KazooDoHigh_0.1s.pcm",
    ),
    ("doc/theory/kazooD_sample.wav", "src/ktl/audio/kazoo_pcm/doot1_pcm.cpp"),
    ("doc/theory/kazooG_sample.wav", "src/ktl/audio/kazoo_pcm/doot2_pcm.cpp"),
    ("src/model_builder/k1_data/KazooDoHigh_0.1s.wav", "src/ktl/audio/kazoo_pcm/doot3_pcm.cpp"),
    ("doc/theory/kazooE_sample.wav", "src/ktl/audio/kazoo_pcm/doot4_pcm.cpp"),
]


def savePcmDataToFile(input_wav_path, output_path):
    sample_rate, data = wavfile.read(input_wav_path)

    with open(output_path, "w") as f:
        flattened = data.reshape(-1)  # Handle multi-channel by flattening

        cpp_output = output_path.endswith(".cpp")
        if cpp_output:
            array_name = (
                "kazoo::pcm_data::"
                + output_path.split("/")[-1].replace(".cpp", "").upper()
            )
            f.write("""#include <ktl/audio/kazoo_pcm.hpp>\n\n""")

            f.write(
                f"const std::array<int16_t, {len(flattened)}> {array_name} = " + "{\n"
            )

        for sample in flattened:
            f.write(f"{sample},\n")

        if cpp_output:
            f.write("};\n")

    print(f"Done. Samples saved to {output_path}")


if __name__ == "__main__":
    for wav_file, pcm_file in FILES_TO_CONVERT:
        savePcmDataToFile(wav_file, pcm_file)
