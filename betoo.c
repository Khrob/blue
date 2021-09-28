#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>


int main (int argc, char **argv)
{
	for (int i=0; i<argc; i++) printf("%s\n", argv[i]);
	printf("hurrah\n");

	av_register_all();

	char *filename = "test.mov";
	AVFormatContext *pFormatContext = avformat_alloc_context();
	avformat_open_input(&pFormatContext, filename, NULL, NULL);
	printf("Format %s, duration %lld us\n", pFormatContext->iformat->long_name, pFormatContext->duration);

	avformat_find_stream_info(pFormatContext,  NULL);


	AVCodec *pCodec = NULL;
  	AVCodecParameters *pCodecParameters =  NULL;
  	int video_stream_index = -1;

	for (int i = 0; i < pFormatContext->nb_streams; i++)
	{
		// AVCodecParameters *pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
		// AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);
		// // specific for video and audio
		// if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO) {
		//   printf("Video Codec: resolution %d x %d", pLocalCodecParameters->width, pLocalCodecParameters->height);
		// } else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO) {
		//   printf("Audio Codec: %d channels, sample rate %d", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
		// }
		// // general
		// printf("\tCodec %s ID %d bit_rate %lld", pLocalCodec->long_name, pLocalCodec->id, pLocalCodecParameters->bit_rate);	
	}

	return 0;
}