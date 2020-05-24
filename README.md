# goodtimes
these are old programs from tildearrow that weren't originally public.

the code of these programs is really ugly.

also, most of these programs won't simply compile on windows for now, but a fix is coming soon.

## Dependencies

varies on program.

- 4channelbypass, peakmeter, tiasynth, vicsynth, lightshow and synth1: jack

- counter, videotests, peakmeter, metronome, stdinaudio, tiasynth, vicsynth, pads, credits, synth1, thegame, timertest: everything allegro

- pads: SDL2 and/or jack

## Operating system support

 Program        | Linux | macOS | Windows
----------------|-------|-------|---------
 4channelbypass |  yes  |  yes  |   no
 counter        |  yes  |  yes  |   yes
 videotests     |  yes  |  yes  |   yes
 peakmeter      |  yes  |  yes  |   no
 metronome      |  yes  |  yes  |   yes
 stdinaudio     |  yes  |  yes  |   yes
 tiasynth       |  yes  |  yes  |   no
 vicsynth       |  yes  |  yes  |   no
 pads           |  yes  |  yes  |   yes
 credits        |  yes  |  yes  |   yes
 lightshow      |  yes  |  yes  |   no
 synth1         |  yes  |  yes  |   no
 thegame        |  yes  |  yes  |   yes
 timertest      |  yes  |  yes  |   yes

## Compiling

### Linux

```
mkdir build
cd build
cmake ..
make
wget http://unifoundry.com/pub/unifont/unifont-12.0.01/font-builds/unifont-12.0.01.ttf
mv unifont-12.0.01.ttf unifont.ttf
cp unifont.ttf text.ttf
cp ../programs/thegame/alexlogo.png ..
```

### macOS

probably same as Linux.

### Windows

really complex process (for now).

1. clone this repository.

2. open cmd.exe, cd to the repo and

```
mkdir build
cd build
cmake ..
```

3. open resulting solution in Visual Studio.

4. Tools > NuGet Package Manager > Package Manager Console, and

```
Install-Package Allegro
```

5. open File Explorer.

6. go to repo directory.

7. create 2 directories: `wininclude` and `winlib`.

8. copy everything from `build\packages\Allegro.5.2.0.0\build\native\include\allegro5\` to `wininclude\allegro5\`.

9. copy everything from `build\packages\AllegroDeps.1.4.0.0\build\native\include\` to `wininclude`.

10. make a directory called `x86` in `winlib`.

11. copy everything from `build\packages\Allegro.5.2.0.0\build\native\v140\win32\bin\` to `winlib\x86\`.

11. copy everything from `build\packages\Allegro.5.2.0.0\build\native\v140\win32\lib\` to `winlib\x86\`.

12. download [http://libsdl.org/release/SDL2-devel-2.0.4-VC.zip].

13. open download location and unzip SDL2-devel-2.0.4-VC.zip.

14. go to `SDL2-devel-2.0.4-VC\SDL2-2.0.4`.

15. copy everything from `include` to `(repoLocation)\wininclude\SDL2\`.

16. copy everything from `lib\x86\` to `(repoLocation)\winlib\x86\`.

17. go back to Visual Studio.

18. Build > Build Solution.

19. download unifont-8.0.01.ttf from [http://unifoundry.com/pub/unifont-8.0.01/font-builds/].

20. open download location.

21. copy unifont-8.0.01.ttf to `(repoLocation)\build\Debug\unifont.ttf`.

22. copy unifont-8.0.01.ttf to `(repoLocation)\build\Debug\text.ttf`.

23. go to repo dir.

24. copy `programs\thegame\alexlogo.png` to `build\`.

25. copy all dll files from `winlib\x86\` to `build\Debug\`.

999. take a little break. enjoy.
