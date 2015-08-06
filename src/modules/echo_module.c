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



int echo_construct(EventLoop * eventLoop_p){
	Echo_context *echo_context_p;
    echo_context_p = ntmalloc(sizeof(Echo_context));
    if (echo_context_p!=NULL){
        echo_context_p->eventLoop_p = eventLoop_p;
        return MODULE_OK; 
    }
    return MODULE_ERR; 
}


int echo_destruct(Echo_context *echo_context_p){
	if(echo_context_p!=NULL){
        if (echo_context_p->message != NULL){
             ntfree(echo_context_p->message);
        }
		ntfree(echo_context_p);
    }

	return MODULE_OK;
}

int echo_package_complete(Echo_context * echo_context_p){
    return 1;
}

int echo_accept (Echo_context * echo_context_p, Client * client_p){
    ntLogging(LOG_DEBUG,"accept client %d", client_p->fd);  
    return STEP_FORWARD;
}

int echo_do_read(Echo_context *echo_context_p, Client *client_p){
    if (DATA_PARSE_SUCCESS != echo_package_compplete(echo_context_p)) {
         return STEP_CYC ;
    }
    return STEP_FORWARD;
}


int echo_do(Echo_context *echo_context_p, Client *client_p){
    ntLogging(LOG_DEBUG,"echo_do");
    return STEP_FORWARD;
}

int echo_do_write(Echo_context *echo_context_p, Client *client_p){
    ntLogging(LOG_DEBUG,"echo_do_write");
    return STEP_FORWARD;
}

int echo_done(Echo_context *echo_context_p){
    ntLogging(LOG_DEBUG,"echo_done" );
	return STEP_OVER;	
}


Module echo_module = { "echo",
	echo_construct,
	echo_destruct,
    echo_accept,
	echo_do_read,
	echo_do,
	echo_do_write,
	echo_done,
};
