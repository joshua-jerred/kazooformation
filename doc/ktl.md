# Kazoo Translation Layer (KTL)

The KTL is responsible for translating between raw data and Kazoo audio. This is the real meat and potatoes of the
project.

![KTL Structure](uml/ktl_structure.png)

```mermaid
---
title: KTL Data to Audio
---

%%flowchart LR
flowchart TD
    Start((" \n ")) -->|" addData(span[uint8_t]) "| AddData
    subgraph AddData
        AddData_Start((" \n ")) --> B
        B(Populate BinaryStream)
        C("Convert binary input data into symbols\n(process BinaryStream into SymbolStream)")
        B --> C
    end

    AddData -->|" encode(output_strategy) "| EncodeData
    subgraph EncodeData
        Modulate("Process Symbols &\nEncode symbol into PCM")
        Filter(Audio Filtering)
        LoadAudioSampleBuffer(Load into audio\nsample buffer)
        Modulate --> Filter --> LoadAudioSampleBuffer --> OutputStrategy
        OutputStrategy{output_strategy selection}
        OutputStrategy -->|" Wav File "| EncodeToWavFile
        OutputStrategy -->|" Pulse Audio "| EncodeToPulseAudio
    end

    EncodeToWavFile(Encode WAV PCM) --> SaveFile(Save to Wav File) --> stop
    EncodeToPulseAudio("Blocking pulse audio API\n(play audio)") --> stop

```