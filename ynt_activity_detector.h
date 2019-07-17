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

#define  YNT_VAD_RESOURCE_FILE   "/usr/local/lib/vad/resources/vad.pb"

#define YNT_VAD_START_OF_INPUT  1
#define YNT_VAD_SPEECH_COMPLETE 2

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/** Events of activity detector */
typedef enum {
	YNT_DETECTOR_EVENT_NONE,       /**< no event occurred */
	YNT_DETECTOR_EVENT_ACTIVITY,   /**< voice activity (transition to activity from inactivity state) */
	YNT_DETECTOR_EVENT_INACTIVITY, /**< voice inactivity (transition to inactivity from activity state) */
	YNT_DETECTOR_EVENT_NOINPUT     /**< noinput event occurred */
} ynt_detector_event_e;

/** Detector states */
typedef enum {
	YNT_DETECTOR_STATE_INACTIVITY,           /**< inactivity detected */
	YNT_DETECTOR_STATE_ACTIVITY_TRANSITION,  /**< activity detection is in-progress */
	YNT_DETECTOR_STATE_ACTIVITY,             /**< activity detected */
	YNT_DETECTOR_STATE_INACTIVITY_TRANSITION /**< inactivity detection is in-progress */
} ynt_detector_state_e;

typedef void (*vad_callback)(ynt_detector_event_e, void*);

/** Activity detector */
typedef struct ynt_activity_detector_s{
    char*            id;
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
	//void*            vad_obj;
	/* current state */
	ynt_detector_state_e state;
	/* duration spent in current state  */
	size_t           duration;
	int              check_flag;
	vad_callback     func;
	void* user_data;
}ynt_activity_detector_t;


/* 全局初始化生成vad对象 */
int ynt_activity_detector_load();

/* 全局释放 */
void ynt_activity_detector_unload();

/** Create activity detector */
ynt_activity_detector_t* ynt_activity_detector_create();

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
ynt_detector_event_e ynt_activity_detector_process(ynt_activity_detector_t *detector, ynt_audionode_t *node, uint32_t node_count);

int ynt_vad_process(ynt_activity_detector_t *detector, ynt_audio_ctl_t* audio_ctl, ynt_audionode_t *audio_node);

#ifdef __cplusplus
}
#endif 


#endif /* YNT_ACTIVITY_DETECTOR_H */
