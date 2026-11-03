# Neon Model D (prototype)
Original monophonic synthesizer inspired by classic three-oscillator subtractive synth architectures. Not affiliated with Behringer or Moog. No original firmware, samples, artwork or circuits copied.

## Features
- 3 independently mixed oscillators, waveform and octave controls, detune and glide
- Four-stage nonlinear ladder-inspired lowpass approximation, cutoff, emphasis and filter contour amount
- Separate amplifier and filter envelopes, noise, drive, LFO pitch and cutoff modulation
- Mono MIDI input, velocity response, automation and preset state saving
- JUCE 8, VST3, Windows x64, GitHub Actions build

## Build
Create a new GitHub repository and upload the files including `.github/workflows/build.yml` **at the repository root**. In Actions, select `Build Neon Model D VST3 Windows` and download the artifact after a successful build. Install the `.vst3` bundle into `C:\Program Files\Common Files\VST3\`.

## Important limitations
This is a **source prototype**, not a tested binary. No audio test or Windows build has been performed. Oscillators are naive (not band-limited), so high notes may alias. Ladder filter is an approximation, not a circuit-accurate model. MIDI note handling is monophonic, last-note priority without a held-note stack. No built-in step sequencer, sync I/O, or hardware connectivity. Parameter wave/octave controls use continuous float parameters rounded internally.
