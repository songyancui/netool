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


#ifndef __MODULE_H
#define __MODULE_H
#include "../dict.h"
#include "../event.h"
#include "../client.h"

#define MODULE_OK 0
#define MODULE_ERR -1
#define DATA_PARSE_SUCCESS 1

#define STEP_FORWARD 1
#define STEP_CYC 2
#define STEP_OVER 3

typedef void (*do_handler)(dict * result);


typedef struct module{
	char * module_name ;

	void * (* _construct)(EventLoop * eventLoop_p);  //it will run when load module
	int *  (*_destruct)(EventLoop * eventLoop_p);                //run when the module canceled

	int (*_accept)(void * module_context, Client *client_p); //callback it when received the message
	int (*_do_read)(void * module_context, Client *client_p); //callback it when received the message
	int (*_do)(void * module_context, Client *client_p); //callback it when received the message
	int (*_do_write)(void * module_context, Client *client_p); //callback it when received the message
	int (*_done)(void * module_context, Client *client_p); //callback it when received the message
} Module;

dict *loadAllModules();

#endif 
