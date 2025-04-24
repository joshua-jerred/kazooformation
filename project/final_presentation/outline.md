# Final Presentation Outline

## Introduction - 20s (00:20)

Hello everyone,

My name is Joshua Jerred and I'm a Computer Science student here.

I am currently working at Miller Electric as an Embedded Software Engineer
in a Co-Op position and I plan to work there full time after graduating.

...

Today I'd like to talk to you all about kazooformation.

## The Problem (Problem Description) 30s (00:50)

Information theory and signal analysis are confusing and intimidating topics.

This leaves the pysical layer of the OSI model as an exercise for the reader.
even though software solutions do exist down there.

I've implemented a few simple modulation and demodulation libraries in the past;

I think they're really neat, but it has been a challenge to explain them to
others as most of the core concepts are obfuscated by big words and
triganometry.

## The Solution [1] 40s (1:30)

This is where the kazoo comes into play.

With a little bit of bluring the lines between what is possible and what is reasonable, I've created a physical layer that you can hear and feel, focusing on core intuition over textbook teaching.

...

I created a software library that translates any serialized data to
and from kazoo sounds using various kazoo models.

Because it's just an abstract physical layer, this could
be used for anything.

### The Solution [2] 30s (2:00)

As an example of this abstract concept, here is a visualization of the ASCII,
letter `H` being translated into kazoo signals using a basic binary model.

It might not be audible to the people in the other room, but this is what the letter
`H` sounds like

> Play `H`

These signals can be used either live via a users microphone and speakers or
they can be stored as audio files for later decoding.

## Library Application Example: Kazoo Connect ??
> 20s (2:20)

As an example application of this library I created a barebones chat client on
top of it. Using this chat client, text can be sent from one computer to
another using only audio peripherals.

No wifi, ethernet, or Bluetooth connection required.

## Future Plans [Org Diagram] ??
<!-- > 15s (2:45) -->

Unforunately I ran out of time and couldn't implement the Kazoo File System
or the Kazoo Compiler, but this wouldn't be too difficult now that the
Kazoo Translation Library is complete.

<!-- Kazoo connect simply uses this physical layer. -->

## Technical Architecture

The translation library was made in C++ using FFTW3 for signal analysis and
pulse audio for live audio input/output.

The most important technical detail of the translation layer is that it has a
generic model structure.

This means that different sounds and detection strategies can be used
depending on the use case.

For example, if you want to transmit kazoo sounds
via the open air you would want to use a binary model that only has two unique
kazoo sounds. If you're storing data in .wav files you would want to use a more
efficient kazoo model.

...

The kazoo connect chat client is a fairly simple application. For the backend
I make a quick and dirty http server using unix tcp sockets; this keeps all
state on the C++ side of things.

The frontend was made using the react router starter app, it's a textbox, button, and a list.

## Lessons Learned

I'm going to gloss over the details of what we're actually looking at here,
just know that the top row shows an ideal signal, and the bottom two rows
show kazoo signals.

In my intro presentation I noted that the kazoo was an inherently noisy
insturment, this turned out to be a massive issue.

No matter the code that I threw at the problem, I couldn't differentiate between signals
well enough to make this work open air without an audio cable.

I eventually had to sit down in front of a microphone, kazoo in hand, and record
cleaner samples.

## Answer any questions

That's kazooformation; any questions?


- hardware idea: 