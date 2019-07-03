#ifndef AUDIOLIST_H
#define AUDIOLIST_H

typedef signed char int8_t;
typedef unsigned char uint8_t; 
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

#define VAD_SAMPLE_SIZE   160  
#define VAD_SAMPLE_COUNT  24
#define VAD_NODE_SIZE     (VAD_SAMPLE_SIZE * VAD_SAMPLE_COUNT)



//#define VAD_SAMPLE_PER_CHUNK   160   /* samplerate:8000, 20ms = 320bytes */
//#define VAD_SAMPLES_PER_REQ    20    /* audio samples per vad req */

//#define VAD_DATA_LEN_PER_REQ   (VAD_SAMPLE_PER_CHUNK * VAD_SAMPLES_PER_REQ)
//#define VAD_SAMPLES_INDEX_MAX  (VAD_SAMPLES_PER_REQ - 1)
//#define VAD_SAMPLES_INDEX_MIN  (0)

enum
{
	YNT_AUDIO_SAMPLE_INIT           = 0x00,
    YNT_AUDIO_SAMPLE_FIRST          = 0x01,
    YNT_AUDIO_SAMPLE_CONTINUE       = 0x02,
    YNT_AUDIO_SAMPLE_LAST           = 0x04,
};


typedef struct ynt_audionode_s{
    char buff[VAD_NODE_SIZE];
    int  offset;  
	struct ynt_audionode_s* pre;
	struct ynt_audionode_s* next;
}ynt_audionode_t;

typedef struct ynt_audio_ctl_s{
	ynt_audionode_t*  head;
	ynt_audionode_t*  cur;
	uint32_t          node_count;
}ynt_audio_ctl_t;

/* function prototype */
ynt_audio_ctl_t * ynt_audiolist_create();

int ynt_audiolist_push_back(ynt_audio_ctl_t *audio_ctl, ynt_audionode_t* node);

int ynt_audiolist_pop_back(ynt_audio_ctl_t *audio_ctl);

void ynt_audiolist_destroy(ynt_audio_ctl_t *audio_ctl);

int ynt_audiolist_write_frame(ynt_audio_ctl_t *audio_ctl, const void* waveData, unsigned int waveLen);

void* ynt_audiolist_merge_memory(ynt_audio_ctl_t *audio_ctl);

void ynt_audiolist_free_memory(void* pcm_addr);

#endif

