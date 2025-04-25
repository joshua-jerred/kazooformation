# Kazooformation Update and Open Questions

## Update

The big news, I attempted the transition from a discrete problem
to a semi-continuous problem. It works! (somewhat).

- Pulse Audio reading and writing is functional
- 3-5x FFTs done across a single symbol, allowing for symbol edge detection
- Byte alignment was done by creating a simple frame/packet structure, if the
packet contents are restricted to 7-bit ASCII (AX.25 does this, for other reasons)
then frame start/end can be unique.
- Up to 4x symbols, already struggling with differentiation.


## Completion Plan
- [ ] 4x symbols (MFSK/AFSK/QKSK)
- [ ] Frame marker A and B symbols (curtis up/down)
  - [ ] Turn on chat link after the volume crosses a threshold or A/B symbols are detected
  - [ ] Start frame reception after this process begins
  - [ ] Symbol alignment *could* be done with i/q processing instead of just throwing FFTs at the problem
- [ ] The model *must* be a continuous process, simply don't clear the context between buffer processing steps


- Model loader
  - Point to WAV files
  - Define with JSON