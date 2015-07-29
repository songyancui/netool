/**
* 
* Copyright (c) 2006-2020, Songyan Cui <songyancui.gmail.com>
* All rights reserved.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   * Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*   * Neither the name of Redis nor the names of its contributors may be used
*     to endorse or promote products derived from this software without
*     specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN	
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/


#include "module.h"
#include "../dict.h"
#include "../adlist.h"
#include "../log.h"


typedef struct echo_event {
	event_base *event_base_ptr;
}Echo_event;

typedef struct echo_message{
	char  message[1024];
	int message_len;
	event * event;
	evutil_socket_t client_fd;
	timeval echo_timeout ;
	int retry;   //TODO try the max retry count
	Task *task;
	event_base *event_base_ptr;	
}Echo_message;


echo_construct(event_base * event){
	Echo_event *echo_event_ptr;
	echo_event_ptr = malloc(sizeof(Echo_module));			
	echo_even_ptr->event_base_ptr = event;
	return echo_event_ptr;
}

int echo_destruct(void* module){
	if(module!=NULL)
		free(module);
	module = NULL;
	return 1;
	
}


void echo_do(void *module, Task *task){
	Echo_event *echo_event_ptr = (Echo_event *)module;
	struct timeval echo_timeout = {3,0};
	Echo_message * echo_message_ptr = malloc(sizeof(Echo_message));
	memset(echo_message_ptr, 0, sizeof(Echo_message));
	
	echo_message_ptr->client_fd = task->client_fd;
	echo_message_ptr->task = task;
	echo_message_ptr->retry = 3;   //TODO  take the value in the config file
	echo_message_ptr->event_base = echo_event_ptr;
	echo_message_ptr->event = event_new(echo_event_ptr->event_base_ptr,task->client_fd,EV_TIMEOUT|EV_READ|EV_PERSIST, echo_do_read_callback,echo_message_ptr);
	event_add(echo_message_ptr->event,&echo_timeout);
}


void echo_do_read_callback(evutil_socket_t client_fd, short what, void * echo_message_ptr){
		sgLogging(LOG_DEBUG,"%s ",__FUNCTION__);
		Echo_message echo_message_p =(Echo_message *) echo_message_ptr	;	
		if(what&EV_TIMEOUT&&echo_message_ptr->retry>0){ 
			echo_message_ptr->retry --;
		}else{
			event_del(echo_message_ptr->event); 
			sgLogging(LOG_WARNING,"%s","echo message  is deleted for expiring");
		}
	
		if(what&EV_READ){
			echo_message_p = channel_receive(client_fd, echo_message_p);
			sgLogging(LOG_DEBUG,"%s %s","recive the message",echo_message_p->message);
			event_del(echo_message_ptr->event); 
			echo_message_ptr->event = event_set(echo_event_ptr->event_base_ptr,task->client_fd,EV_TIMEOUT|EV_WRITE|EV_PERSIST, echo_do_write_callback,echo_message_ptr);
			event_add(echo_message_ptr->event,&echo_timeout);
				
		}
}

void echo_do_write_callback(evutil_socket_t client_fd, short what, void * echo_message_ptr){
	sgLogging(LOG_DEBUG,"%s",__FUNCTION__);
	Echo_message echo_message_p = (Echo_message *) echo_message_ptr;
	if(what&EV_TIMEOUT&&echo_message_ptr->retry>0){ 
		echo_message_ptr->retry --;
	}else{
		event_del(echo_message_ptr->event); 
		sgLogging(LOG_WARNING,"%s","echo message  is deleted for expiring");
	}
	if(what&EV_WRITE){
		channel_push(client_fd, echo_message_p);
		event_del(echo_message_p->event);
			
	}	
	
}

int echo_done(Task *task, Echo_message *echo_message_ptr){
	if(echo_message_ptr !=NULL){
		free(echo_message_ptr);
		echo_message_ptr = NULL;
	}
	
	if(task !=NULL){
		free(task);
		task = NULL;
	}
	return 1;	
}


Module echo_module = { "echo",
	event_base *event_base_ptr,
	
	echo_construct,
	echo_destruct,
	
	echo_do,
	echo_done,

};
