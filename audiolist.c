#include <stdio.h>
#include <stdlib.h>
#include "audiolist.h"
#include <string.h>



/* function definition */
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
      //printf("node_count:%u  blank_idx:%u \n", audio_ctl->node_count, node->blank_idx);
      node = audio_ctl->head;
	  audio_ctl->head = audio_ctl->head->next;
	  free(node);
	  audio_ctl->node_count--;
    }

	free(audio_ctl);
	audio_ctl = NULL;
}



