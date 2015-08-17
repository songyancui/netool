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


#include <stdio.h>
#include <stdlib.h>
#include "modules.h"
#include "../dict.h"
#include "../adlist.h"
#include "../event.h"
#include "../log.h"
#include "../mm.h"
#include "../client.h"
#include "../io.h"

typedef struct echo_message{
	char  message[1024];
	int message_len;
}Echo_message;

typedef struct echo_context {
	EventLoop *eventLoop_p;
    Echo_message *message;
}Echo_context;


int echo_construct(Module * module_p, EventLoop * eventLoop_p){
    ntLogging(LOG_DEBUG,"echo module construct ");
	Echo_context *echo_context_p;
    echo_context_p = ntmalloc(sizeof(Echo_context));
    module_p->module_context = echo_context_p;

    if (echo_context_p!=NULL){
        echo_context_p->eventLoop_p = eventLoop_p;
        echo_context_p->message = NULL;
        return STEP_FORWARD; 
    }
    return STEP_FORWARD; 
}


int echo_destruct(void  * echo_context_p){
    ntLogging(LOG_DEBUG,"echo module destruct ");
	if(echo_context_p!=NULL){
		ntfree(echo_context_p);
    }

	return STEP_FORWARD;
}

int echo_package_complete(Client *client_p){
    client_p->recv_msg_times ++;
    if (client_p->recv_msg_times > MAX_CLIENT_RECV_MSG_TIMES){
        return DATA_PARSE_FAILED;
    }

    if (client_p->recv_msg_len > 0 && client_p->recv_msg !=NULL){
        ntLogging(LOG_DEBUG,"data :%s", client_p->recv_msg);
        if (client_p->recv_msg[client_p->recv_msg_len-1] == '\n') {
            return DATA_PARSE_SUCCESS; 
        } 
    }
    return DATA_PARSE_UNSUCCESS;
}

int echo_accept (void * echo_context_p, Client * client_p){
    ntLogging(LOG_DEBUG,"accept client %d", client_p->fd);  
    return STEP_FORWARD;
}

int echo_do_read(void  *echo_context_p, Client *client_p){
    if (DATA_PARSE_SUCCESS != echo_package_complete(client_p)) {
        ntLogging(LOG_DEBUG,"reading data did not completed" );
         return STEP_CYC ;
    }
    return STEP_FORWARD;
}


int echo_do(void  *echo_context_p, Client *client_p){
    ntLogging(LOG_DEBUG,"echo_do");
    
    client_p->send_msg = ntmalloc(client_p->recv_msg_len);
    if (client_p->send_msg == NULL){
        return STEP_CYC;
    }
    client_p->send_msg_len = client_p->recv_msg_len;
    ntmemcpy(client_p->send_msg, client_p->recv_msg, client_p->recv_msg_len);

    return STEP_FORWARD;
}

int echo_do_write(void *echo_context_p, Client *client_p){
    ntLogging(LOG_DEBUG,"echo_do_write");
    return STEP_FORWARD;
}

int echo_done(void  *echo_context_p, Client *client_p){
    ntLogging(LOG_DEBUG,"echo_done" );
	return STEP_OVER;	
}


Module echo_module = { "echo_module",
	echo_construct,
	echo_destruct,
    echo_accept,
	echo_do_read,
	echo_do,
	echo_do_write,
	echo_done,
    NULL,
};
