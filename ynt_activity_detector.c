#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ynt_activity_detector.h"
#include "ynt_vad_api.h"

//#include "apt_log.h"

int ynt_activity_detector_vad_object_init()
{
    /* 生成vad对象 */
	vad_obj = YNT_vad_new(YNT_VAD_RESOURCE_FILE);
	if(vad_obj == NULL){
		return -1;
	}
	printf("ynt_activity_detector_vad_object_init vad_obj(%p)\n", vad_obj);
    return 0;
}

void ynt_activity_detector_vad_object_release()
{
	YNT_vad_release(vad_obj);
}

/** Create activity detector */
ynt_activity_detector_t* ynt_activity_detector_create()
{
	ynt_activity_detector_t *detector = (ynt_activity_detector_t *)malloc(sizeof(ynt_activity_detector_t));
	detector->level_threshold         = 12;   /* 0 .. 255 */
	detector->recognize_timeout       = 300;  /* 0.3 s */
	detector->speech_complete_timeout = 300;  /* 0.3 s */
	detector->noinput_timeout         = 5000; /* 5 s */
	detector->duration                = 0;
	detector->state = YNT_DETECTOR_STATE_INACTIVITY;

	return detector;
}

void ynt_activity_detector_destroy(ynt_activity_detector_t *detector)
{
	if(detector != NULL){
        free(detector);
    }
}

/** Reset activity detector */
void ynt_activity_detector_reset(ynt_activity_detector_t *detector)
{
	detector->duration = 0;
	detector->state = YNT_DETECTOR_STATE_INACTIVITY;
}

/** Set threshold of voice activity (silence) level */
void ynt_activity_detector_level_set(ynt_activity_detector_t *detector, size_t level_threshold)
{
	detector->level_threshold = level_threshold;
}

/** Set noinput timeout */
void ynt_activity_detector_noinput_timeout_set(ynt_activity_detector_t *detector, size_t noinput_timeout)
{
	detector->noinput_timeout = noinput_timeout;
}

/** Set timeout required to trigger speech (transition from inactive to active state) */
void ynt_activity_detector_speech_timeout_set(ynt_activity_detector_t *detector, size_t recognize_timeout)
{
	detector->recognize_timeout = recognize_timeout;
}

/** Set timeout required to trigger silence (transition from active to inactive state) */
void ynt_activity_detector_silence_timeout_set(ynt_activity_detector_t *detector, size_t speech_complete_timeout)
{
	detector->speech_complete_timeout = speech_complete_timeout;
}

static void ynt_activity_detector_state_change(ynt_activity_detector_t *detector, ynt_detector_state_e state)
{
	detector->duration = 0;
	detector->state = state;
	//apt_log(YNT_LOG_MARK, APT_PRIO_INFO, "Activity Detector state changed [%d]",state);
}

ynt_detector_state_e ynt_activity_detector_state_get(ynt_activity_detector_t *detector)
{
    if(detector == NULL) {
        return YNT_DETECTOR_STATE_INACTIVITY;
    }

	return detector->state;
}

static size_t ynt_activity_detector_level_calculate(const char* data, size_t len)
{
	size_t sum = 0;
	size_t count = len/2;
	const int16_t *cur = (int16_t *)data;
	const int16_t *end = cur + count;

	for(; cur < end; cur++) {
		if(*cur < 0) {
			sum -= *cur;
		}
		else {
			sum += *cur;
		}
	}

	return sum / count;
}

ynt_detector_event_e ynt_activity_detector_process(ynt_activity_detector_t *detector, ynt_audionode_t *node, uint32_t          node_count)
{
	ynt_detector_event_e det_event = YNT_DETECTOR_EVENT_NONE;
    int vad_result         = 0;
	uint32_t speech_time   = 0;

	/* first, calculate current activity level of processed frame */
	vad_result = YNT_audio_single_vad(vad_obj, 1, 8000, 16, (void*)node->buff, VAD_NODE_SIZE, 0);

	printf("YNT_audio_single_vad vad_obj(%p) result(%d)\n", vad_obj, vad_result);
	if(detector->state == YNT_DETECTOR_STATE_INACTIVITY) {
		if(vad_result) {
			/* start to detect activity */
		    det_event = YNT_DETECTOR_EVENT_ACTIVITY;
			ynt_activity_detector_state_change(detector,YNT_DETECTOR_STATE_ACTIVITY);
		}
		else {
			detector->duration += YNT_CODEC_FRAME_TIME;
			if(detector->duration >= detector->noinput_timeout) {
				/* detected noinput */
			   printf("ynt_activity_detector_process duration(%d) noinput_timeout(%d)\n", detector->duration, detector->noinput_timeout);
				det_event = YNT_DETECTOR_EVENT_NOINPUT;
			}
		}
	}
	//else if(detector->state == YNT_DETECTOR_STATE_ACTIVITY_TRANSITION) {
	//	if(vad_result) {
	//		detector->duration += YNT_CODEC_FRAME_TIME;
	//		/* default speech_duration */
	//		if(detector->duration >= detector->speech_duration) {
	//			/* finally detected activity */
	//			det_event = YNT_DETECTOR_EVENT_ACTIVITY;
	//			ynt_activity_detector_state_change(detector,YNT_DETECTOR_STATE_ACTIVITY);
	//		}
	//	}
	//	else {
	//		/* fallback to inactivity */
	//		ynt_activity_detector_state_change(detector,YNT_DETECTOR_STATE_INACTIVITY);
	//	}
	//}
	else if(detector->state == YNT_DETECTOR_STATE_ACTIVITY) {
		if(vad_result) {
			detector->duration += YNT_CODEC_FRAME_TIME;
			/* 说话时长达到阈值, 主动结束 */
		    speech_time = (node_count + 1) * YNT_CODEC_FRAME_TIME;
			if(speech_time >= VAD_SPEECH_TIME_MAX){
			    det_event = YNT_DETECTOR_EVENT_INACTIVITY;
				ynt_activity_detector_state_change(detector, YNT_DETECTOR_STATE_INACTIVITY);
			}
		}
		else {
			/* start to detect inactivity */
			ynt_activity_detector_state_change(detector,YNT_DETECTOR_STATE_INACTIVITY_TRANSITION);
		}
	}
	else if(detector->state == YNT_DETECTOR_STATE_INACTIVITY_TRANSITION) {
		if(vad_result) {
			/* fallback to activity */
			ynt_activity_detector_state_change(detector,YNT_DETECTOR_STATE_ACTIVITY);
		}
		else {
			detector->duration += YNT_CODEC_FRAME_TIME;
			if(detector->duration >= detector->speech_complete_timeout) {
				/* detected inactivity */
				det_event = YNT_DETECTOR_EVENT_INACTIVITY;
				ynt_activity_detector_state_change(detector,YNT_DETECTOR_STATE_INACTIVITY);
			}
		}
	}
	
	return det_event;
}

int ynt_vad_process(ynt_activity_detector_t *detector, ynt_audio_ctl_t* audio_ctl, ynt_audionode_t *audio_node)
{
    int result = 0;
	int ret    = 0;
	ynt_detector_event_e det_event = YNT_DETECTOR_EVENT_NONE;

	result = YNT_audio_single_vad(detector->vad_obj, 1, 8000, 16, audio_node->buff, VAD_NODE_SIZE, 0);

    if(result < 0) {
		return -1;
	}
	
	if(detector->state == YNT_DETECTOR_STATE_INACTIVITY) {
		if(result) {
			/* start to detect activity */
			ynt_activity_detector_state_change(detector, YNT_DETECTOR_STATE_ACTIVITY_TRANSITION);
		}
		else {
			detector->duration += YNT_CODEC_FRAME_TIME;
		}
	}
	else if(detector->state == YNT_DETECTOR_STATE_ACTIVITY_TRANSITION) {
		if(result) {
			detector->duration += YNT_CODEC_FRAME_TIME;
			/* default speech_duration */
			if(detector->duration >= detector->recognize_timeout) {
				/* finally detected activity */
			   det_event = YNT_DETECTOR_EVENT_ACTIVITY;
				ynt_activity_detector_state_change(detector, YNT_DETECTOR_STATE_ACTIVITY);
			}
		}
		else {
			/* fallback to inactivity */
			ynt_activity_detector_state_change(detector, YNT_DETECTOR_STATE_INACTIVITY);
		}
	}
	else if(detector->state == YNT_DETECTOR_STATE_ACTIVITY) {
		if(result) {
			detector->duration += YNT_CODEC_FRAME_TIME;
			/* 说话时长达到阈值可以判定为inactivity并送asr识别 */
			if(audio_ctl->node_count*YNT_CODEC_FRAME_TIME >= VAD_SPEECH_TIME_MAX){
			    det_event = YNT_DETECTOR_EVENT_INACTIVITY;
				ynt_activity_detector_state_change(detector, YNT_DETECTOR_STATE_INACTIVITY);
			}
		}
		else {
			/* start to detect inactivity */
			ynt_activity_detector_state_change(detector, YNT_DETECTOR_STATE_INACTIVITY_TRANSITION);
		}
	}
	else if(detector->state == YNT_DETECTOR_STATE_INACTIVITY_TRANSITION) {
		if(result) {
			/* fallback to activity */
			ynt_activity_detector_state_change(detector, YNT_DETECTOR_STATE_ACTIVITY);
		}
		else {
			detector->duration += YNT_CODEC_FRAME_TIME;
			if(detector->duration >= detector->speech_complete_timeout) {
				/* detected inactivity */
			    //switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_INFO, "%s detected inactivity.\n", switch_core_session_get_uuid(da_request->session));
			    det_event = YNT_DETECTOR_EVENT_INACTIVITY;
				ynt_activity_detector_state_change(detector, YNT_DETECTOR_STATE_INACTIVITY);
			}
		}
	}

    /* 依据vad状态机的状态， 执行下一步动作 */
	if(detector->state == YNT_DETECTOR_STATE_ACTIVITY || detector->state == YNT_DETECTOR_STATE_INACTIVITY_TRANSITION){
         /* 检查链表是否存在 */
	     if(audio_ctl == NULL){
			audio_ctl = ynt_audiolist_create();
		 }
		 if(audio_ctl == NULL){
			 //switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "%s ynt_audiolist_create fail.\n", switch_core_session_get_uuid(da_request->session));
			 return;
		 }
		 /* 将该节点放入链表 */
	     ret = ynt_audiolist_push_back(audio_ctl, audio_node);
	     if(ret < 0){
             //switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "%s ynt_audiolist_push_back err(%d).\n", switch_core_session_get_uuid(da_request->session), ret);
		 }

		/* 生成新节点，初始化 */
		audio_node = (ynt_audionode_t*)malloc(sizeof(ynt_audionode_t));
		memset(audio_node, 0x0, sizeof(ynt_audionode_t)); 
		
	}
	/* 送出状态变更事件,并且状态机处于inactivity状态 */
	else if(detector->state == YNT_DETECTOR_STATE_INACTIVITY &&  det_event == YNT_DETECTOR_EVENT_INACTIVITY){
		/* 将链表送语音识别 */
		//ynt_asr_process(da_request);
    }
	else {
      /* 未检出声音, 清空vad节点音频数据 */
         memset(audio_node, 0x0, sizeof(ynt_audionode_t));
	}
	return;
}


