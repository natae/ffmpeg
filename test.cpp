#include "test.hh"
int main(int argc, char** argv)
{
	avformat_network_init();
	av_register_all();
	avcodec_register_all();
	
	AVFormatContext *pFormatCtx;
	int i;
	AVCodecContext *pCodecCtx, *aCodecCtx;
	int videoStream, audioStream;
	AVCodec *pCodec, *aCodec;
	AVFrame *pFrame, *pFrameRGB;

	uint8_t *buffer;
	int numBytes;

	int frameFinished;
	AVPacket packet;
	int head = 0;
	int tail = 0;
	int end_buf = 0;


	struct SwsContext *img_convert_ctx;
	
	SDL_Surface *screen;
	SDL_Overlay *bmp;
	SDL_Rect rect;
	SDL_Event event;
	SDL_AudioSpec wanted_spec, spec;

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
	std::cout<<"gd"<<std::endl;	
	
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
	
	wanted_spec.freq = aCodecCtx->sample_rate;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.channels = aCodecCtx->channels;
	wanted_spec.silence = 0;
	wanted_spec.samples = 1024;
	wanted_spec.callback = audio_callback;
	wanted_spec.userdata = aCodecCtx;
	std::cout<<"gd"<<std::endl;
		
	if(SDL_OpenAudio(&wanted_spec, &spec) < 0)
	{
		fprintf(stderr, "SDL_OpenAudio: %s\n", SDL_GetError());
		return -1;
	}
	
	packet_queue_init(&audioq);
	SDL_PauseAudio(0);
	
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
	
	aCodec = avcodec_find_decoder(aCodecCtx->codec_id);
	if(aCodec == NULL)
	{
		std::cout<<"Undupported coedc"<<std::endl;
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


	i=0;
	while(av_read_frame(pFormatCtx, &packet) >= 0)
	{
		if(packet.stream_index == videoStream)
		{
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

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

//				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pict.data, pict.linesize);

				SDL_UnlockYUVOverlay(bmp);



//				rect.x = 0;
//				rect.y = 0;
//				rect.w = pCodecCtx->width;
//				rect.h = pCodecCtx->height;
				//			std::cout<<"hi"<<std::endl;
//				SDL_DisplayYUVOverlay(bmp, &rect);


				
				 
			}
		}
		/*
		else if(packet.stream_index == audioStream)
		{
			packet_queue_put(&audioq, &packet);
		}
		else
			av_free_packet(&packet);
		*/


	}
	av_free_packet(&packet);
	SDL_PollEvent(&event);

	switch(event.type)
	{
		case SDL_QUIT:
			quit = 1;
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
void packet_queue_init(PacketQueue *q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->mutex = SDL_CreateMutex();
	q->cond = SDL_CreateCond();
}
int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
	AVPacketList *pkt1;
	if(av_dup_packet(pkt) < 0)
	{
		return -1;
	}
	pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
	if(!pkt1)
		return -1;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;

	SDL_LockMutex(q->mutex);

	if(!q->last_pkt)
		q->first_pkt = pkt1;
	else
		q->last_pkt->next = pkt1;
	q->last_pkt = pkt1;
	q->nb_packets++;
	q->size = q->size + pkt1->pkt.size;
	SDL_CondSignal(q->cond);

	SDL_UnlockMutex(q->mutex);
	return 0;
}
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
{
	AVPacketList *pkt1;
	int ret;

	SDL_LockMutex(q->mutex);

	for(;;)
	{
		if(quit)
		{
			ret = -1;	
			break;
		}
		pkt1 = q->first_pkt;
		if(pkt1)
		{
			q->first_pkt = pkt1->next;
			if(!q->first_pkt)
				q->last_pkt = NULL;
			q->nb_packets--;
			q->size = q->size - pkt1->pkt.size;
			*pkt = pkt1->pkt;
			av_free(pkt1);
			ret = 1;
			break;
		}
		else if(!block)
		{
			ret = 0;
			break;
		}
		else
			SDL_CondWait(q->cond, q->mutex);
	}
	SDL_UnlockMutex(q->mutex);
	return ret;
}
int decode_interrupt_cb(void)
{
	return quit;
}
void audio_callback(void *userdata, Uint8 *stream, int len)
{
	AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
	int len1, audio_size;

	static uint8_t audio_buf[(MAX_AUDIO_FRAME_SIZE * 3) / 2];
	static unsigned int audio_buf_size = 0;
	static unsigned int audio_buf_index = 0;

	while(len > 0) 
	{
		if(audio_buf_index >= audio_buf_size) 
		{
			/* We have already sent all our data; get more */
			audio_size = audio_decode_frame(aCodecCtx, audio_buf,
					sizeof(audio_buf));
			if(audio_size < 0)
			{
				/* If error, output silence */
				audio_buf_size = 1024;
				memset(audio_buf, 0, audio_buf_size);
			} 
			else 
			{
				audio_buf_size = audio_size;
			}
			audio_buf_index = 0;
		}
		len1 = audio_buf_size - audio_buf_index;
		if(len1 > len)
			len1 = len;
		memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
		len -= len1;
		stream += len1;
		audio_buf_index += len1;
	}
}
int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size) {

	static AVPacket pkt;
	static uint8_t *audio_pkt_data = NULL;
	static int audio_pkt_size = 0;

	int len1, data_size;

	for(;;) {
		while(audio_pkt_size > 0) 
		{
			data_size = buf_size;
			len1 = avcodec_decode_audio3(aCodecCtx, (int16_t *)audio_buf, &data_size, 
					&pkt);
			if(len1 < 0) 
			{
				/* if error, skip frame */
				audio_pkt_size = 0;
				break;
			}
			audio_pkt_data += len1;
			audio_pkt_size -= len1;
			if(data_size <= 0) 
			{
				/* No data yet, get more frames */
				continue;
			}
			/* We have data, return it and come back for more later */
			return data_size;
		}
		if(pkt.data)
			av_free_packet(&pkt);

		if(quit) 
		{
			return -1;
		}

		if(packet_queue_get(&audioq, &pkt, 1) < 0) 
		{
			return -1;
		}
		audio_pkt_data = pkt.data;
		audio_pkt_size = pkt.size;
	}
}
