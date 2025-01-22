# Kazoo Translation Layer (KTL)

The KTL is responsible for translating between raw data and Kazoo audio. This is the real meat and potatoes of the
project.

It is implemented as a C++ library, via CMake as an interface library, with the target name `kazoo_translation_layer`.

## Structure / Design

```puml
@startuml ktl_structure

title Kazoo Translation Layer Structure

object input_binary_data
input_binary_data : Binary File
input_binary_data : C Array
input_binary_data : String

object pulse_audio_io
object wav_file

package ktl {
  class kazoo::TranslationLayer {
    +TranslationLayer(output_strategy)
    +addData(span<uint8_t> data)
    +encode()
  }
'  kazoo::TranslationLayer <--> input_binary_data
  input_binary_data  <--> kazoo::TranslationLayer
  kazoo::TranslationLayer <--> SymbolStream
  kazoo::TranslationLayer <--> BinaryStream

  class BinaryStream {}
  
  class SymbolStream {}
  abstract SymbolModel {
     +getSymbolValue(token) const
     +getSymbolToken(value) const

     -const map<token, value>
     -const map<value, token>
   }
  SymbolStream <--> SymbolModel

  class Transcoder {}
  
  
  
  kazoo::TranslationLayer <--> Transcoder
  
'  BinaryStream <--> SymbolStream
'  SymbolStream <--> Transcoder

  Transcoder <--> pulse_audio_io
  Transcoder <--> wav_file
}

@enduml
```

```mermaid
---
title: KTL Data to Audio
---

flowchart LR
%%flowchart TD
    Start((" \n ")) -->|" addData(span[uint8_t]) "| AddData
    subgraph AddData
        AddData_Start((" \n ")) --> B
        B(Populate BinaryStream)
        C("Convert binary input data into symbols\n(BinaryStream to SymbolStream)")
        B --> C
    end

    AddData -->|" encode(output_strategy) "| EncodeData
    subgraph EncodeData
        Modulate("Modulate Symbols &\nEncode symbol into PCM &\nAudio Filtering")
        LoadAudioSampleBuffer(Load into audio\nsample buffer)
        Modulate --> LoadAudioSampleBuffer --> OutputStrategy
        OutputStrategy{output\nstrategy}
        OutputStrategy -->|" wav "| EncodeToWavFile
        OutputStrategy -->|" pulse "| EncodeToPulseAudio

    end
    stop((("\n")))
    EncodeToWavFile("Save to Wav File") --> stop
    EncodeToPulseAudio("Blocking pulse audio API\n(play audio)") --> stop

```

```puml
@startuml ktl_audio_to_data

title Kazoo Translation Layer Audio to Data

start

if (Input Mode) then (wav file)
  :Load From .wav;
else (Pulse)
  :PulseAudio input stream;
endif

:Demodulate Symbols;

:Convert Symbols to BinaryStream;

stop

@enduml
```