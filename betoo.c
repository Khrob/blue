
#include "platform.h"

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>


int main (int argc, char **argv)
{
	for (int i=0; i<argc; i++) printf("%s\n", argv[i]);

	char *filename = argv[1];

	int err = 0;

	AVFormatContext *format_context = avformat_alloc_context();
	AVCodecContext *codec_context = NULL;
	AVCodec *codec = NULL;

	avformat_open_input(&format_context, filename, NULL, NULL);

	int stream = av_find_best_stream(format_context, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
	
	printf("stream %d, Format %s, duration %lld us\n", stream, format_context->iformat->long_name, format_context->duration);

	if (codec == NULL) { printf ("Couldn't read the codec\n"); exit(-1000); }

	codec_context = avcodec_alloc_context3(codec);
	err = avcodec_parameters_to_context(codec_context, format_context->streams[stream]->codecpar);
	if (err) { printf("Couldn't get the codec parameters\n"); exit(-1000); }

	printf("%d,%d %lld\n", codec_context->width, codec_context->height, codec_context->bit_rate);

	err = avcodec_open2(codec_context, codec, NULL);
	if (err) { printf("Couldn't open the stream\n"); exit(-1000); }

	AVPacket *packet = av_packet_alloc();
	err = av_read_frame (format_context, packet);
	if (err) { printf("av_read_frame: %d\n", err); exit (-1000); }

	printf ("frame details: size: %d duration: %lld\n", packet->size, packet->duration);

	err = avcodec_send_packet(codec_context, packet);
	if (err) { printf("avcodec_send_packet: %d\n", err); exit (-1000); }

	AVFrame *frame = av_frame_alloc();
	for(;;) {
		err = avcodec_receive_frame(codec_context, frame);
		if (err<0) break;
	}


	printf("got frame, maybe?\n");

	return 0;
}

typedef struct Video
{
	int width, height;
	int bit_rate;
	int frame_count;
} 
Video;