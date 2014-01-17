#include <iostream>
   extern "C"
   {
   #include <libavformat/avformat.h>
   #include <libavcodec/avcodec.h>
   #include <libswscale/swscale.h>
   #include <SDL.h>
   #include <SDL_thread.h>
   }
#define MAX_AUDIO_FRAME_SIZE 192000
typedef struct PacketQueue
{
	AVPacketList *first_pkt, *last_pkt;
	int nb_packets;
	int size;
	SDL_mutex *mutex;
	SDL_cond *cond;
}PacketQueue;
int decode_interrupt_cb(void);
void audio_callback(void *userdata, Uint8 *stream, int len);
int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size);
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block);
int packet_queue_put(PacketQueue *q, AVPacket *pkt);
void packet_queue_init(PacketQueue *q);
void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame);

int quit = 0;
PacketQueue audioq;

