#include <stdio.h>
#include <stdlib.h>
#include "audiolist.h"
#include <string.h>
#include <pthread.h>


/* function definition */

ynt_audionode_t* ynt_audionode_create(ynt_audio_ctl_t *audio_ctl)
{
    ynt_audionode_t* node = NULL;
	ynt_audionode_pool_dynamic_t* dynamic_pool = audio_ctl->dynamic_pool;
	node = (ynt_audionode_t*)malloc(dynamic_pool->size);
	memset(node, 0x0, sizeof(ynt_audionode_t));
	
	node->node_size = audio_ctl->node_size;

	return node;
}

#if 0
int ynt_audionode_insert(ynt_audionode_t* node, void* data, unsigned int size)
{
    if(node == NULL || data == NULL || size != VAD_SAMPLE_SIZE)
    {
        return -1;
    }
	
   	/* vad节点装满 */
	if(node->offset >= VAD_NODE_SIZE)
	{
        return -2;
	}	
	memcpy(node->buff + node->offset, data, size);
	node->offset += size;

	return 0;
}
#endif

audiolist_bool_t ynt_audionode_is_full(ynt_audionode_t* node)
{
    if(node == NULL)
    {  
        return TRUE;
    }

    //节点未装满，返回false
	if(node->offset < node->node_size)
	{
	    return FALSE;
	}

	//节点装满，返回 true
	return TRUE;
}

void ynt_audionode_clear(ynt_audionode_t* node)
{
	node->offset = 0;
	node->next   = NULL;
	node->pre    = NULL;

	//memset(node, 0x0, sizeof(ynt_audionode_t));
}

void ynt_audionode_destroy(ynt_audionode_t* node)
{
	if(node == NULL){
       return; 
	}
	free(node);
	node = NULL;
}

ynt_audio_ctl_t* ynt_audiolist_create(uint32_t base_time, uint32_t microseconds_per_packet, uint32_t decoded_bytes_per_packet)
{
    uint32_t node_size = base_time / microseconds_per_packet * decoded_bytes_per_packet;
	ynt_audio_ctl_t *audio_ctl = NULL;
	ynt_audionode_pool_dynamic_t* dynamic_pool = NULL;

	audio_ctl = (ynt_audio_ctl_t*)malloc(sizeof(ynt_audio_ctl_t));
    memset(audio_ctl, 0x0, sizeof(ynt_audio_ctl_t));
	dynamic_pool = (ynt_audionode_pool_dynamic_t*)malloc(sizeof(ynt_audionode_pool_dynamic_t));
	dynamic_pool->size = node_size + sizeof(ynt_audionode_t);
	audio_ctl->dynamic_pool = dynamic_pool;
	
	audio_ctl->microseconds_per_packet = microseconds_per_packet;
	audio_ctl->decoded_bytes_per_packet = decoded_bytes_per_packet;
	audio_ctl->node_size = node_size;

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

    //该节点已经放入链表所以将指针置为NULL，避免误用
	node = NULL;
	
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

audiolist_bool_t ynt_audiolist_clear_node(ynt_audio_ctl_t *audio_ctl)
{
    ynt_audionode_t* node = NULL;

    if(!audio_ctl)
		return FALSE;
    
    while(audio_ctl->node_count)
    {
		node = audio_ctl->head;
		audio_ctl->head = audio_ctl->head->next;
		free(node);
		audio_ctl->node_count--;
    }

    audio_ctl->head = NULL;
    audio_ctl->cur  = NULL;
	
	return TRUE;
}

void ynt_audiolist_destroy(ynt_audio_ctl_t *audio_ctl)
{
    ynt_audionode_t* node = NULL;

    if(!audio_ctl)
		return;
    
    while(audio_ctl->node_count)
    {
		node = audio_ctl->head;
		audio_ctl->head = audio_ctl->head->next;
		free(node);
		audio_ctl->node_count--;
    }
    free(audio_ctl->dynamic_pool);
	free(audio_ctl);
	audio_ctl = NULL;
}

int ynt_audionode_write(ynt_audionode_t *node, const void* waveData, unsigned int waveLen)
{
    if(node == NULL){
        return -1;
    }
	
	if(node->offset < 0){
        return -2;
	}

	if(waveData == NULL){
        return -3;
	}

	if((node->offset + waveLen) > node->node_size){
        return -4;
	}
    
    memcpy(node->buff + node->offset, waveData, waveLen);
    node->offset += waveLen;
	
    return 0; 
}

#if 0
int ynt_audiolist_write_frame(ynt_audio_ctl_t *audio_ctl, const void* waveData, unsigned int waveLen)
{
	int ret = 0;

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

    memcpy(audio_ctl->cur->buff + audio_ctl->cur->offset, waveData, waveLen);
    audio_ctl->cur->offset += waveLen;

    /* vad节点装满 */
	if(audio_ctl->cur->offset == VAD_NODE_SIZE)
	{
		//node = (ynt_audionode_t*)malloc(sizeof(ynt_audionode_t));
		//memset(node, 0x0, sizeof(ynt_audionode_t));
	    ynt_audiolist_push_back(audio_ctl, ynt_audionode_create());
		ret = 1;
	}
	
    return ret; 
}
#endif

void* ynt_audiolist_merge_memory(ynt_audio_ctl_t *audio_ctl)
{
    unsigned int audio_size = 0;
	char *ptr			    = NULL;
	void* pcm_addr		    = NULL;
	ynt_audionode_t* pos    = NULL;

	audio_size = audio_ctl->node_count * audio_ctl->node_size;
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
		memcpy(ptr, pos->buff, audio_ctl->node_size);
			
		/* move the node position */
		pos = pos->next;
			
		/* move the pointer */
		ptr += audio_ctl->node_size;
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


