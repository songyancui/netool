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
#include "../adlist.h"
#include "../event.h"
#include "../client.h"

#define MODULE_OK 0
#define MODULE_ERR -1

//parse message package and check completation
#define DATA_PARSE_SUCCESS 1
#define DATA_PARSE_UNSUCCESS -1
#define DATA_PARSE_FAILED -2

//modules return values
#define STEP_FORWARD 1
#define STEP_REWIND 2
#define STEP_CYC 3
#define STEP_OVER 4 


#define MAX_MODULES_SIZE 16

#define HOOK_MODULES_CONSTRUCT(eventLoop_p) hook_modules_construct(eventLoop_p)
#define HOOK_MODULES_DESTRUCT  hook_modules_destruct()
#define HOOK_MODULES_ACCEPT(client_p) hook_modules_accept(client_p)
#define HOOK_MODULES_READING(client_p) hook_modules_do_read(client_p)
#define HOOK_MODULES_PROCESSING(client_p) hook_modules_do(client_p)
#define HOOK_MODULES_WRITING(client_p) hook_modules_do_write(client_p)
#define HOOK_MODULES_DONE(client_p) hook_modules_done(client_p)

typedef void (*do_handler)(dict * result);

dict *allModules;
list * modules;
typedef struct module{
	char * module_name ;

	int  (* _construct)(struct module * module_p , EventLoop * eventLoop_p);  //it will run when load module
	int  (*_destruct)(void * module_context);                //run when the module canceled

	int (*_accept)(void * module_context, Client *client_p); //callback it when accept client 
	int (*_do_read)(void * module_context, Client *client_p); //callback it when reading the message ,not complete
	int (*_do)(void * module_context, Client *client_p); //callback it when received the message completely
	int (*_do_write)(void * module_context, Client *client_p); //callback it when writing the message
	int (*_done)(void * module_context, Client *client_p); //callback it after writing complete 
    void * module_context;  //module context
} Module;

void loadAllModules();

#endif 
