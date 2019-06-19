#ifndef AUDIOLIST_H
#define AUDIOLIST_H

typedef signed char int8_t;
typedef unsigned char uint8_t;
 
typedef short int16_t;
typedef unsigned short uint16_t;
 
typedef int int32_t;
typedef unsigned int uint32_t;

#define VAD_SAMPLE_PER_CHUNK   320   /** samplerate:8000, 20ms = 320bytes */
/* vad need 320ms data */
#define VAD_SAMPLES_PER_REQ    20    /** audio samples per vad req */

#define VAD_DATA_LEN_PER_REQ   (VAD_SAMPLE_PER_CHUNK * VAD_SAMPLES_PER_REQ)
#define VAD_SAMPLES_INDEX_MAX  (VAD_SAMPLES_PER_REQ - 1)

/** audio data */
typedef struct ynt_audio_buf_s
{
	uint8_t vad_sample[VAD_SAMPLE_PER_CHUNK];
}ynt_audio_buf_t;

union ynt_u_audio
{
	uint8_t u_data[VAD_DATA_LEN_PER_REQ];
	ynt_audio_buf_t buf[VAD_SAMPLES_PER_REQ];
};

typedef struct ynt_audionode_s{
	union ynt_u_audio    audio_data;
	uint8_t              blank_idx;    
	struct ynt_audionode_s*  pre;
	struct ynt_audionode_s*  next;
}ynt_audionode_t;

typedef struct ynt_audio_ctl_s{
	ynt_audionode_t    *head;
	ynt_audionode_t    *cur;
	uint32_t            node_count;
}ynt_audio_ctl_t;



/* function prototype */
ynt_audio_ctl_t * ynt_audiolist_create();

int ynt_audiolist_push_back(ynt_audio_ctl_t *audio_ctl, ynt_audionode_t* node);

int ynt_audiolist_pop_back(ynt_audio_ctl_t *audio_ctl);

void ynt_audiolist_destroy(ynt_audio_ctl_t *audio_ctl);


#endif

