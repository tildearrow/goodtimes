# soundtracker

![like, argh.](ohplease.png)

> sorry. yet another heap corruption bug. <

this is my first program (excluding the tutorials).

note: after you compile this program, put unifont.ttf (you can find it by searching "GNU Unifont") and logo.png in the build directory.

text below is from a info.txt file i had.

This is a chiptune tracker.

## Features.
- You can work with up to 4 chips at once for a total of 32 channels.
- You can import IT/S3M/MOD files directly into the program.

## Tracker's Specifications.
This is a format specification!


bytable[8][256][256], allows for setting up the envelope tables, up to 512kb

bytable[table][indextab][position], where

table=0-7, 0=volume, 1=pitch, 2=pitch*256, 3=cutoff, 4=resonance-filter-mode, 5=shape, 6=duty

position=0-255, 0-252=envelope, 253=length, 254=loop-position, 255=release-position


pat[256][256][8][5], pattern table, 2560kb

pat[patid][patpos][channel][bytepos], where

patid=0-255

patpos=0-255

channel=0-7

bytepos=0-4, 0=note, 1=instrument-reverse, 2=volume-effect, 3=effect, 4=effect-value

## Playback.
Playback is done like this:

###(Next Tick)
1. Update all envelope positions.
2. Apply effects if required.

###(Next Row)
1. Reset the tick counter.
2. If there is a new note:
  1. Reset all the envelopes, unless there's a Gxx effect in such row.
  2. Change the note value, unless there's a Gxx effect in such row.
3. If there is a new volume value:
  1. Set note volume.
4. If there is any effect in the effect column:
  1. Process such effect.

###(Next Effect)
- Axx: Change the song speed.
- Bxx: Stop this pattern and go to pattern XX.
- Cxx: Skip to next pattern.
- Dxx: Fade in/out the note's volume, while not in Next Row.
- Exx: Decrease the frequency value and set the portamento memory value for further E00s.
- Fxx: Same as Exx but increase it instead.
- Gxx: Portamento with a note.
- Hxy: Apply vibrato effects.
- Ixy: Apply tremor effects.
- Jxy: Apply arpeggio effects.
- Kxy: Slide+Vibrato.
- Lxy: Slide+Porta.
- Mxx: Set channel volume.
- Nxx: Channel volume slide.
- Oxx: Sets the position for ALL envelopes to XX.
- Pxx: Panning slides.
- Qxy: Apply retrigger effects.
- Rxy: Apply tremolo effects.
- Sxy: Apply any special commands if required.
- Txx: Set tempo (not supported).
- Uxy: Apply fine vibrato effects.
- Vxx: 
- Wxx: 
- Xxx: Set channel panning.
- Yxy: Panning tremolo.
- Zxx: Apply MIDI macro. However they are not stored in the resulting file.

## File Format's Specifications.
check format.txt.

## Note Periods.
no longer used.

note| freq   | period
----|-------|------
C-0 | 16,3525| 18346
C#0 | 17,325 | 17316
D-0 | 18,355 | 16344
D#0 | 19,445 | 15428
E-0 | 20,6025| 14561
F-0 | 21,8275| 13744
F#0 | 23,125 | 12973
G-0 | 24,5   | 12245
G#0 | 25,9575| 11557
A-0 | 27,5   | 10909
A#0 | 29,135 | 10297
B-0 | 30,8675| 9719

# <~>
