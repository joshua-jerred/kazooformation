"""Convert wav files to PCM data lists for use in C++ code.

Run from this dir
"""

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
    (
        "src/model_builder/k1_data/KazooDoHigh_0.1s.wav",
        "src/ktl/audio/kazoo_pcm/doot3_pcm.cpp",
    ),
    ("doc/theory/kazooE_sample.wav", "src/ktl/audio/kazoo_pcm/doot4_pcm.cpp"),
    ("src/ktl/models/k2/kazooA.wav", "src/ktl/models/k2/kazoo_A_pcm.cpp"),
    ("src/ktl/models/k2/kazooD.wav", "src/ktl/models/k2/kazoo_D_pcm.cpp"),
    ("src/ktl/models/k2/kazooE.wav", "src/ktl/models/k2/kazoo_E_pcm.cpp"),
    ("src/ktl/models/k2/kazooG.wav", "src/ktl/models/k2/kazoo_G_pcm.cpp"),
    ("doc/k3_model/output/doot_1.wav", "src/ktl/models/k3/k3_doot_low.cpp"),
    ("doc/k3_model/output/doot_2.wav", "src/ktl/models/k3/k3_doot_high.cpp"),
]


def savePcmDataToFile(input_wav_path, output_path):
    sample_rate, data = wavfile.read(input_wav_path)

    with open(output_path, "w") as f:
        flattened = data.reshape(-1)  # Handle multi-channel by flattening

        cpp_output = output_path.endswith(".cpp")
        if cpp_output:
            base_array_name = "pcm_data"
            if "/k2/" in output_path:
                base_array_name = "k2_pcm_data"

            array_name = (
                f"kazoo::{base_array_name}::"
                + output_path.split("/")[-1].replace(".cpp", "").upper()
            )
            if "/k2/" in output_path:
                f.write("""#include <ktl/models/k2/k2_pcm.hpp>\n\n""")
            else:
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
