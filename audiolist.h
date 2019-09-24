#ifndef AUDIOLIST_H
#define AUDIOLIST_H

typedef signed char int8_t;
typedef unsigned char uint8_t; 
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

#define  USE_FREESWITCH

#ifdef USE_MRCP
//mrcp的尺寸
#define VAD_SAMPLE_SIZE      160  
#define VAD_SAMPLE_COUNT     24
#define VAD_SAMPLE_TIME_BASE 10
#endif

#ifdef USE_FREESWITCH
#define VAD_SAMPLE_SIZE      320  
#define VAD_SAMPLE_COUNT     8    //调整
#define VAD_SAMPLE_TIME_BASE 20
#endif

#define VAD_NODE_SIZE        (VAD_SAMPLE_SIZE * VAD_SAMPLE_COUNT)
#define VAD_NODE_TIME        (VAD_SAMPLE_TIME_BASE * VAD_SAMPLE_COUNT)

#define VAD_SPEECH_TIME_MAX  (20000)     /* 设定说话最长时间20秒 */

#define VAD_EARLY_MEDIA_TIME_MAX  (4000)  /* 设定需要的早期媒体音频最大长度为4000ms */
//YNTDA_FRAME_TIME_MAX

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

ynt_audionode_t* ynt_audionode_create();

void ynt_audionode_empty(ynt_audionode_t* node);

void ynt_audionode_destroy(ynt_audionode_t* node);

ynt_audio_ctl_t* ynt_audiolist_create();

int ynt_audiolist_push_back(ynt_audio_ctl_t *audio_ctl, ynt_audionode_t* node);

int ynt_audiolist_pop_back(ynt_audio_ctl_t *audio_ctl);

void ynt_audiolist_destroy(ynt_audio_ctl_t *audio_ctl);

int ynt_audiolist_write_frame(ynt_audio_ctl_t *audio_ctl, const void* waveData, unsigned int waveLen);

void* ynt_audiolist_merge_memory(ynt_audio_ctl_t *audio_ctl);

void ynt_audiolist_free_memory(void* pcm_addr);

/* 将一帧音频写入节点
* 返回值 1:节点写满  0:节点未写满
*/
int ynt_audionode_write(ynt_audionode_t *node, const void* waveData, unsigned int waveLen);

/* 将一帧音频写入链表*/
int ynt_audiolist_write_frame(ynt_audio_ctl_t *audio_ctl, const void* waveData, unsigned int waveLen);


#endif

