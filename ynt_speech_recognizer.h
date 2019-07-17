#ifndef YNT_SPEECH_RECOGNIZER_H
#define YNT_SPEECH_RECOGNIZER_H

#include "audiolist.h"
#include "ynt_globals.h"
#include "ynt_asr_client_api.h"
/**
 * @file ynt_speech_recognizer.h
 * @brief ynt automatic speech recognizer
 */ 

typedef void (*asr_result_callback)(const char*, void*);

typedef struct {
    char* id;
    unsigned int model_id;
    engine_type_t type;
	char* result;
	asr_result_callback result_func;
	void* user_data;
}ynt_speech_recognizer_t;

ynt_speech_recognizer_t* ynt_speech_recognizer_create();

void ynt_speech_recognizer_destroy(ynt_speech_recognizer_t* recognizer);

char* ynt_asr_process(ynt_audio_ctl_t* audio_ctl, ynt_speech_recognizer_t* recognizer);


#endif