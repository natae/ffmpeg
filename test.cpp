#include <iostream>
extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <SDL.h>
#include <SDL_thread.h>
}
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);
int main(int argc, char** argv)
{
	avformat_network_init();
	av_register_all();
	avcodec_register_all();

	AVFormatContext *pFormatCtx;
	int i;
	AVCodecContext *pCodecCtx, *aCodecCtx;
	int videoStream, audioStream;
	AVCodec *pCodec;
	AVFrame *pFrame, *pFrameRGB;

	uint8_t *buffer;
	int numBytes;

	int frameFinished;
	AVPacket packet;
	AVPicture pict_buffer[20];
	int pict_buffer_index = 0;

	struct SwsContext *img_convert_ctx;
	
	SDL_Surface *screen;
	SDL_Overlay *bmp;
	SDL_Rect rect;
	SDL_Event event;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
	{
		std::cout<<"Can't not init"<<std::endl;
		return -1;
	}
	if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL) != 0)
	{
		std::cout<<"Can't find file"<<std::endl;
		return -1;
	}
	
	if(avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{	
		std::cout<<"Can't find info"<<std::endl;	
		return -1;
	}

	av_dump_format(pFormatCtx, 0, argv[1], 0);
	
	videoStream = -1;
	audioStream = -1;
	for(i=0;i<pFormatCtx->nb_streams;i++)
	{
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO && videoStream<0)
			videoStream = i;
			
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO && audioStream<0)
			audioStream = i;	
	}
	if(videoStream == -1)
		return -1;
	if(audioStream == -1)
		return -1;

	pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	aCodecCtx = pFormatCtx->streams[audioStream]->codec;
	
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec == NULL)
	{
		std::cout<<"Unsupprted codec"<<std::endl;
		return -1;
	}
	if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0)   
	{
		std::cout<<"Can't open codec"<<std::endl;
		return -1;
	}


	pFrame = av_frame_alloc();
	screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 24, 0);
	if(!screen)
	{
		std::cout<<"screen error"<<std::endl;
	}

	bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height, SDL_YV12_OVERLAY, screen);
	pFrameRGB = av_frame_alloc();
	if(pFrameRGB == NULL)
	{
		return -1;
	}
	
	numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
	buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	avpicture_fill((AVPicture*)pFrameRGB, buffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

	i=0;
	while(av_read_frame(pFormatCtx, &packet) >= 0)
	{
		if(packet.stream_index == videoStream)
		{
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
		}
		if(frameFinished)
		{
			SDL_LockYUVOverlay(bmp);

			AVPicture pict;
			pict.data[0] = bmp->pixels[0];
			pict.data[1] = bmp->pixels[2];
			pict.data[2] = bmp->pixels[1];
			pict.linesize[0] = bmp->pitches[0];
			pict.linesize[1] = bmp->pitches[2];
			pict.linesize[2] = bmp->pitches[1];

			img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
			if(img_convert_ctx == NULL)
			{
				exit(1);
			}
			sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pict.data, pict.linesize);

			SDL_UnlockYUVOverlay(bmp);

			rect.x = 0;
			rect.y = 0;
			rect.w = pCodecCtx->width;
			rect.h = pCodecCtx->height;
//			std::cout<<"hi"<<std::endl;
//			SDL_DisplayYUVOverlay(bmp, &rect);
/*
			if(++i<=5)
				SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
*/
		}
	}
	av_free_packet(&packet);
	SDL_PollEvent(&event);

	switch(event.type)
	{
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
			break;
		default:
			break;
	}	

	av_free(buffer);
	av_free(pFrameRGB);

	av_free(pFrame);

	avcodec_close(pCodecCtx);

	av_close_input_file(pFormatCtx);

	return 0;
}
void SaveFrame(AVFrame* pFrame, int width, int height, int iFrame)
{
	FILE *pFile;
	char szFilename[32];
	int y;

	sprintf(szFilename, "frame%d.ppm", iFrame);
	pFile=fopen(szFilename, "wb");
	if(pFile == NULL)
		return;

	fprintf(pFile, "P6\n%d %d\n255\n", width, height);

	for(y=0;y<height;y++)
		fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

	fclose(pFile);
}
