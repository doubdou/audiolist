#ifndef AUDIOLIST_H
#define AUDIOLIST_H

#define FALSE 0
#define TRUE (!FALSE)

typedef int audiolist_bool_t;
typedef signed char int8_t;
typedef unsigned char uint8_t; 
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

#define  USE_FREESWITCH

/**
#ifdef USE_MRCP
//mrcp的尺寸
#define VAD_SAMPLE_SIZE      160  
#define VAD_SAMPLE_COUNT     32
#define VAD_SAMPLE_TIME_BASE 10
#endif

#ifdef USE_FREESWITCH
//FreeSWITCH尺寸
#define VAD_SAMPLE_SIZE      320  
#define VAD_SAMPLE_COUNT     16    //调整
#define VAD_SAMPLE_TIME_BASE 20
#endif

#define VAD_NODE_SIZE        (VAD_SAMPLE_SIZE * VAD_SAMPLE_COUNT)
#define VAD_NODE_TIME        (VAD_SAMPLE_TIME_BASE * VAD_SAMPLE_COUNT)
*/

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

/** Dynamic allocation of messages (no actual pool exist)*/
typedef struct ynt_audionode_pool_dynamic_t ynt_audionode_pool_dynamic_t;

struct ynt_audionode_pool_dynamic_t {
	uint32_t size;
};

typedef struct ynt_audionode_s{
	struct ynt_audionode_s* pre;
	struct ynt_audionode_s* next;
	int offset;  
	int node_size;
	char buff[1];
}ynt_audionode_t;

typedef struct ynt_audio_ctl_s{
	ynt_audionode_t*  head;
	ynt_audionode_t*  cur;
	ynt_audionode_pool_dynamic_t* dynamic_pool;
	uint32_t          decoded_bytes_per_packet; //VAD_SAMPLE_SIZE
	uint32_t          microseconds_per_packet;  //VAD_SAMPLE_TIME_BASE
	uint32_t          node_count;
	uint32_t          node_size;
}ynt_audio_ctl_t;

/* function prototype */

ynt_audionode_t* ynt_audionode_create(ynt_audio_ctl_t *audio_ctl);

#if 0
int ynt_audionode_insert(ynt_audionode_t* node, void* data, unsigned int size);
#endif

audiolist_bool_t ynt_audionode_is_full(ynt_audionode_t* node);

void ynt_audionode_clear(ynt_audionode_t* node);

void ynt_audionode_destroy(ynt_audionode_t* node);

ynt_audio_ctl_t* ynt_audiolist_create(uint32_t base_time, uint32_t microseconds_per_packet, uint32_t decoded_bytes_per_packet);

int ynt_audiolist_push_back(ynt_audio_ctl_t *audio_ctl, ynt_audionode_t* node);

int ynt_audiolist_pop_back(ynt_audio_ctl_t *audio_ctl);

audiolist_bool_t ynt_audiolist_clear_node(ynt_audio_ctl_t *audio_ctl);

void ynt_audiolist_destroy(ynt_audio_ctl_t *audio_ctl);

int ynt_audionode_write(ynt_audionode_t *node, const void* waveData, unsigned int waveLen);

#if 0
int ynt_audiolist_write_frame(ynt_audio_ctl_t *audio_ctl, const void* waveData, unsigned int waveLen);
#endif

void* ynt_audiolist_merge_memory(ynt_audio_ctl_t *audio_ctl);

void ynt_audiolist_free_memory(void* pcm_addr);

/* 将一帧音频写入节点
* 返回值 1:节点写满  0:节点未写满
*/
int ynt_audionode_write(ynt_audionode_t *node, const void* waveData, unsigned int waveLen);

/* 将一帧音频写入链表*/
int ynt_audiolist_write_frame(ynt_audio_ctl_t *audio_ctl, const void* waveData, unsigned int waveLen);


#endif

