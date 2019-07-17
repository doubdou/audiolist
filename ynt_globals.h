#ifndef  YNT_GLOBALS_H
#define  YNT_GLOBALS_H

typedef enum {
	ENGINE_TYPE_NONE,
 	ENGINE_TYPE_ASR_YNT_ASYNC = 1,	/* 1:意能通流式识别   */
	ENGINE_TYPE_ASR_YNT_SYNC,  	    /* 2:意能通一句话识别 */
	ENGINE_TYPE_ASR_ALY_ASYNC,	    /* 3:阿里流式识别     */
	ENGINE_TYPE_ASR_ALY_SYNC,  	    /* 4:阿里一句话识别   */	
	ENGINE_TYPE_VAD_DEFAULT = 10,   /* 10:默认的能量值的vad检测 */
	ENGINE_TYPE_VAD_YNT ,           /* 11:意能通基于深度学习的vad检测*/
	ENGINE_TYPE_VAD_WEBRTC,         /* 12:webrtc的vad检测 */
}engine_type_t;

#endif
