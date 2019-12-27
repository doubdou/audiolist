#ifndef YNT_SPEECH_RECOGNIZER_H
#define YNT_SPEECH_RECOGNIZER_H

#include "audiolist.h"
//#include "ynt_globals.h"
#include "ynt_asr_client_api.h"
/**
 * @file ynt_speech_recognizer.h
 * @brief ynt automatic speech recognizer
 */ 

typedef struct ynt_speech_recognizer_config_s ynt_speech_recognizer_config_t;

struct ynt_speech_recognizer_config_s {
	char*           server_ip;
	unsigned short  server_port;
    char*           category;
	char*           hotword;
	int             pushgateway;
};

typedef void (*asr_result_callback)(const char*, void*);

typedef struct {
    char* id;
	const void* asr_client;
    unsigned int model_id;
	unsigned int normalize;
	char* hotword;
	char* result;
	asr_result_callback result_func;
	void* user_data;
}ynt_speech_recognizer_t;

int ynt_speech_recognizer_model_id_set(ynt_speech_recognizer_t* recognizer, unsigned int model_id);

int ynt_speech_recognizer_normalize_set(ynt_speech_recognizer_t* recognizer, unsigned int normalize);

ynt_speech_recognizer_t* ynt_speech_recognizer_create(const void* asr_client, ynt_speech_recognizer_config_t* asr_conf);

void ynt_speech_recognizer_destroy(ynt_speech_recognizer_t* recognizer);

int ynt_asr_process(ynt_audio_ctl_t* audio_ctl, ynt_speech_recognizer_t* recognizer);


#endif
