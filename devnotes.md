**blue**

Wheel Reinvention Jam Notes

Days -n
-------

I had already been planning a handmade-style video editor, and had put together a very simple macos app that opens a metal window, takes some mouse input and draws some rectangles. As far as platform layers and gui libraries go, it's... barely functional. But there you go.

Day 0
-----

Was expecting the jam to kick off on my Tuesday, but of course handmade folks are distributed all over the world, so getting started late on my West Australian Monday. 

Decided to take the sensible route of using ffmpeg to get moving faster. Seems like it covers damn near everything I want in the short term, as well as probably longer term too.

- homebrew installed latest ffmpeg
- made a small c program and build.sh that links against the ffmpeg libraries
- following https://github.com/leandromoreira/ffmpeg-libav-tutorial to try and open a video file and pull out some kind of useful information about it.
- hit linker errors, solved by linking -lm and -lz, obviously. 
- hit more linker errors, solved by linking macos frameworks (VideoDecodeAcceleration, CoreVideo, CoreFoundation)
- hit still more linker errors, solved by linking -lx264
- following more of the tutorial, apparently AVCodecParameters isn't linked or is deprecated or something.
- 



