#ifndef YNT_ACTIVITY_DETECTOR_H
#define YNT_ACTIVITY_DETECTOR_H

/**
 * @file ynt_activity_detector.h
 * @brief YNT Voice Activity Detector
 */ 
#include "stddef.h"
#include "audiolist.h"
#include "ynt_globals.h"

#define  YNT_CODEC_FRAME_TIME_BASE   (VAD_SAMPLE_TIME_BASE)  
#define  YNT_CODEC_FRAME_TIME        (YNT_CODEC_FRAME_TIME_BASE * VAD_SAMPLE_COUNT)

#define  YNT_VAD_RESOURCE_FILE   "/usr/local/lib/vad-check/resources/vad.pb"

#define YNT_VAD_START_OF_INPUT  1
#define YNT_VAD_SPEECH_COMPLETE 2

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/** Events of activity detector */
typedef enum {
	YNT_DETECTOR_EVENT_NONE,             /**< no event occurred */
	YNT_DETECTOR_EVENT_ACTIVITY,         /**< voice activity (transition to activity from inactivity state) */
	YNT_DETECTOR_EVENT_INACTIVITY,       /**< voice inactivity (transition to inactivity from activity state) */
	YNT_DETECTOR_EVENT_FORCE_INACTIVITY, /**< voice inactivity (transition to inactivity from activity state) */
	YNT_DETECTOR_EVENT_NOINPUT           /**< noinput event occurred */
} ynt_detector_event_e;

/** Detector states */
typedef enum {
	YNT_DETECTOR_STATE_INACTIVITY,           /**< inactivity detected */
	YNT_DETECTOR_STATE_ACTIVITY_TRANSITION,  /**< activity detection is in-progress */
	YNT_DETECTOR_STATE_ACTIVITY,             /**< activity detected */
	YNT_DETECTOR_STATE_INACTIVITY_TRANSITION /**< inactivity detection is in-progress */
} ynt_detector_state_e;


typedef ynt_detector_event_e (*vad_callback)(void*, ynt_audionode_t *, uint32_t);

typedef enum{
    YNT_AUDIO_STATE_INIT,
    YNT_AUDIO_STATE_ACTIVE,
    YNT_AUDIO_STATE_COMPLETED,
    YNT_AUDIO_STATE_TIMEOUT,
}ynt_audio_stat_t;


/** Activity detector */
typedef struct ynt_activity_detector_s{
    int              id;
	engine_type_t    type;
	/* voice activity (silence) level threshold */
	size_t           level_threshold;
	/* period of activity required to complete transition to active state */
	size_t           recognize_timeout;
	/* period of inactivity required to complete transition to inactive state */
	size_t           speech_complete_timeout;
	/* noinput timeout */
	size_t           noinput_timeout;
	char*            start_input_timers;         
	/* current state */
	ynt_detector_state_e state;
	/* duration spent in current state  */
	size_t           duration;
	int              check_flag;
	int energy;      //能量阈值（默认设置为0）
    float thresh;    //vad检测阈值（默认设置为0.5，需要加强抗噪可以调整为0.6）
	vad_callback     func;
	void* user_data;
}ynt_activity_detector_t;


/* 全局初始化生成vad对象 */
int ynt_activity_detector_load();

/* 全局释放 */
void ynt_activity_detector_unload();

/** Create activity detector */
ynt_activity_detector_t* ynt_activity_detector_create(unsigned int channels, unsigned int rate, int choice,int energy, float thresh);

void ynt_activity_detector_destroy(ynt_activity_detector_t *detector);

/** Reset activity detector */
void ynt_activity_detector_reset(ynt_activity_detector_t *detector);

/** Set threshold of voice activity (silence) level */
void ynt_activity_detector_level_set(ynt_activity_detector_t *detector, size_t level_threshold);

/** Set noinput timeout */
void ynt_activity_detector_noinput_timeout_set(ynt_activity_detector_t *detector, size_t noinput_timeout);

/** Set timeout required to trigger speech (transition from inactive to active state) */
void ynt_activity_detector_speech_timeout_set(ynt_activity_detector_t *detector, size_t speech_timeout);

/** Set timeout required to trigger silence (transition from active to inactive state) */
void ynt_activity_detector_silence_timeout_set(ynt_activity_detector_t *detector, size_t silence_timeout);

ynt_detector_state_e ynt_activity_detector_state_get(ynt_activity_detector_t *detector);

/** Process current audio node, return detected event if any */
ynt_detector_event_e ynt_activity_detector_default_process(void* obj, ynt_audionode_t *node, uint32_t node_count);

ynt_detector_event_e ynt_activity_detector_process(void* obj, ynt_audionode_t *node, uint32_t node_count);
#if 0
int ynt_vad_process(ynt_activity_detector_t *detector, ynt_audio_ctl_t* audio_ctl, ynt_audionode_t *audio_node);
#endif

#ifdef __cplusplus
}
#endif 


#endif /* YNT_ACTIVITY_DETECTOR_H */
