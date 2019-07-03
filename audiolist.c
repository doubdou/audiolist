#include <stdio.h>
#include <stdlib.h>
#include "audiolist.h"
#include <string.h>
#include <pthread.h>


/* function definition */

ynt_audionode_t* ynt_audionode_create()
{
    ynt_audionode_t* node = NULL;
	node = (ynt_audionode_t*)malloc(sizeof(ynt_audionode_t));
	memset(node, 0x0, sizeof(ynt_audionode_t));

	return node;
}

ynt_audio_ctl_t* ynt_audiolist_create()
{
	ynt_audio_ctl_t *audio_ctl = NULL;

	audio_ctl = (ynt_audio_ctl_t*)malloc(sizeof(ynt_audio_ctl_t));

    memset(audio_ctl, 0x0, sizeof(ynt_audio_ctl_t));

    return audio_ctl;
}


int ynt_audiolist_push_back(ynt_audio_ctl_t *audio_ctl, ynt_audionode_t* node)
{
	if(audio_ctl == NULL || node == NULL){
        return -1;
	}

    if(audio_ctl->head == NULL){
	    audio_ctl->head = node;
		audio_ctl->head->next = NULL;
	    audio_ctl->head->pre  = NULL;
	    audio_ctl->cur = node;
        audio_ctl->node_count++;

		return 0;
	}
    audio_ctl->cur->next = node;

	node->pre = audio_ctl->cur;
	node->next = NULL;

	audio_ctl->cur = node;
	
	audio_ctl->node_count++;
	
    return 0;  
}

int ynt_audiolist_pop_back(ynt_audio_ctl_t *audio_ctl)
{
	ynt_audionode_t* node = NULL;

	if(audio_ctl == NULL || node == NULL){
        return -1;
	}
    node = audio_ctl->cur;
    if(node == NULL){
        return 0;
	}
   
	audio_ctl->cur = node->pre;

	audio_ctl->cur->next = NULL;

	free(node);

	audio_ctl->node_count--;

    return 0;  

}


void ynt_audiolist_destroy(ynt_audio_ctl_t *audio_ctl)
{
  
    ynt_audionode_t* node = NULL;
    
    while(audio_ctl->node_count)
    {
      node = audio_ctl->head;
	  audio_ctl->head = audio_ctl->head->next;
	  free(node);
	  audio_ctl->node_count--;
    }

	free(audio_ctl);
	audio_ctl = NULL;
}

/* 将一帧音频写入链表*/
int ynt_audiolist_write_frame(ynt_audio_ctl_t *audio_ctl, const void* waveData, unsigned int waveLen)
{
	ynt_audionode_t* node = NULL;

    if(audio_ctl == NULL){
         return -1;
    }
	if(waveData == NULL){
        return -2;
	}

	if(waveLen != VAD_SAMPLE_SIZE){
        return -3;
	}
    
    if(audio_ctl->head == NULL){
		//node = (ynt_audionode_t*)malloc(sizeof(ynt_audionode_t));
		//memset(node, 0x0, sizeof(ynt_audionode_t));
	    ynt_audiolist_push_back(audio_ctl, ynt_audionode_create());
    }
	
	if(audio_ctl->cur->offset == VAD_NODE_SIZE)
	{
		//node = (ynt_audionode_t*)malloc(sizeof(ynt_audionode_t));
		//memset(node, 0x0, sizeof(ynt_audionode_t));
	    ynt_audiolist_push_back(audio_ctl, ynt_audionode_create());
	}

    memcpy(audio_ctl->cur->buff + audio_ctl->cur->offset, waveData, waveLen);
    audio_ctl->cur->offset += waveLen;

	//memcpy((void*)&audio_ctl->cur->audio_data.buf[audio_ctl->cur->blank_idx], waveData, waveLen);
	//audio_ctl->cur->blank_idx++;
	
    return 1; 
}


void* ynt_audiolist_merge_memory(ynt_audio_ctl_t *audio_ctl)
{
    unsigned int audio_size = 0;
	char *ptr			    = NULL;
	void* pcm_addr		    = NULL;
	ynt_audionode_t* pos    = NULL;

	audio_size = audio_ctl->node_count * VAD_NODE_SIZE;
	ptr = malloc(audio_size);
		
	if(ptr == NULL) {
	     return NULL;
	} else {
		/* 保存内存地址 */
	    pcm_addr = ptr;
	}
		
	pos = audio_ctl->head;
	while(pos)
	{
		memcpy(ptr, pos->buff, VAD_NODE_SIZE);
			
		/* move the node position */
		pos = pos->next;
			
		/* move the pointer */
		ptr += VAD_NODE_SIZE;
	}
	
    return pcm_addr;
}

void ynt_audiolist_free_memory(void* pcm_addr)
{
    if(pcm_addr == NULL){
       return; 
	}
	free(pcm_addr);
}


