__blue__

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
- Tried to find a more up to date tutorial, without success.

Day 1
-----

- watched https://www.youtube.com/watch?v=fk1bxHi6iSI 
- updated to ffmpeg 4:3 (might as well be on the latest version when I start)
- getting width,height and bitrate for the stream codec! 
- create a frame packet (AVPacket seems like a good thing to prealloc a bunch of these at the start of the program?)
- mixing in some knowledge from http://dranger.com/ffmpeg/tutorial01.html to actually decode a frame. 






__Reading a video file's format__

AVFormatContext *format_context = avformat_alloc_context();
avformat_open_input(&format_context, filename, NULL, NULL);
AVCodec *pCodec = NULL;
int stream = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);



