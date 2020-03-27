#include "ynt_speech_recognizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#undef YNT_DEBUG_AUDIO_DATA


int ynt_speech_recognizer_model_id_set(ynt_speech_recognizer_t* recognizer, unsigned int model_id)
{
    if(NULL == recognizer)
    {
        return -1;
    }
	recognizer->model_id = model_id;

	return 0;
}

int ynt_speech_recognizer_normalize_set(ynt_speech_recognizer_t* recognizer, unsigned int normalize)
{
    if(NULL == recognizer)
    {
        return -1;
    }
	recognizer->normalize = normalize;

	return 0;
}


ynt_speech_recognizer_t* ynt_speech_recognizer_create(const void* asr_client, ynt_speech_recognizer_config_t* asr_conf)
{
   ynt_speech_recognizer_t* recognizer = (ynt_speech_recognizer_t*)malloc(sizeof(ynt_speech_recognizer_t));
   memset(recognizer, 0x0, sizeof(ynt_speech_recognizer_t));

   recognizer->hotword = asr_conf->hotword;
   recognizer->asr_client = asr_client;

   return recognizer;
}

void ynt_speech_recognizer_destroy(ynt_speech_recognizer_t* recognizer)
{
    if(recognizer != NULL)
    {
        free(recognizer);
		recognizer = NULL;
    }
}

#if 0
int ynt_asr_process(ynt_audio_ctl_t* audio_ctl, ynt_speech_recognizer_t* recognizer)
{	
    char* rslt              = NULL;
	unsigned char* pcm_addr = NULL;
	unsigned int pcm_len    = 0;
    int err_code            = 0;

#ifdef YNT_DEBUG_AUDIO_DATA
	char filename[1024];
	FILE* audio_out  = NULL;
	int write_size	 = 0;
	time_t t;
	struct tm * lt;
	time (&t);
	lt = localtime (&t);
	sprintf(filename, "/tmp/ynt_asr_process_%p_%04d%02d%02d%02d%02d%02d.pcm", (void*)audio_ctl, 
				lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
#endif

	pcm_addr = ynt_audiolist_merge_memory(audio_ctl);
	if(pcm_addr == NULL){
		ynt_audiolist_destroy(audio_ctl);
		return -1;
	}
	
	pcm_len = audio_ctl->node_count * audio_ctl->node_size;

#ifdef YNT_DEBUG_AUDIO_DATA
	/* file to write to */
	audio_out = fopen(filename, "w");
	if(audio_out == NULL)
	{
		printf("debug file fopen error.\n");
	} 
	else 
	{
		write_size = fwrite(pcm_addr, 1, pcm_len, audio_out);
		fclose(audio_out);
		if(write_size != pcm_len) 
		{
			printf("debug file write data err, write_size(%d) audio_size(%d)\n", write_size, pcm_len);
		}
	}
#endif
    err_code = ynt_asr_client_speech_recognizer(recognizer->asr_client,
		                                    1, 
		                                    8000, 
		                                    16, 
		                                    recognizer->model_id, 
		                                    0, 
		                                    recognizer->hotword, 
		                                    pcm_addr, 
		                                    pcm_len, 
		                                    recognizer->normalize,
		                                    &rslt);
    if(err_code != 0)
    {
       	printf("ynt asr recognize error(error code: %d)\n", err_code);
		return -2;
    }else {
		//asr结果回调函数
		recognizer->result_func(rslt, recognizer->user_data);
    }
   
	if (NULL != rslt)
	{
		free(rslt);
	}
	ynt_audiolist_free_memory(pcm_addr);

	/* 销毁该链表,生成新链表 */
	ynt_audiolist_destroy(audio_ctl);
	audio_ctl = ynt_audiolist_create();

    return 0;
}
#endif

