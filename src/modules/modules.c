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
#include "../log.h"
#include "../dict.h"
#include "../ntconfig.h"
#include "echo_module.c"

#define REGISTE_MODULE(module)\
	listAddNodeTail(modules, module);\
    ntLogging(LOG_DEBUG,"registe_module:%s", module->module_name)

#define ADD_MODULES(m)\
    dictAdd(allModules, m.module_name, &m);\
    ntLogging(LOG_DEBUG,"add_modules:%s", m.module_name)

void registeModules(){

    modules = listCreate();
	ntLogging(LOG_DEBUG,"load all modules");
    
    int i=0;
    char * module_name;
    Module * module_st = NULL;
    for(i; g_server_config.modules[i]!= NULL; i++){
        module_name = g_server_config.modules[i] ;
        if ((module_st = (Module *)dictFetchValue(allModules, module_name)) != NULL){
	        REGISTE_MODULE(module_st);
        }else{
            ntLogging(LOG_WARNING,"there IS not $s before adding allModules", module_name);
            continue;
        }
    } 

}

void loadAllModules(){
    allModules = dictCreate(&dictTypeHeapStringCopyKey, NULL);
    ADD_MODULES(echo_module);

    registeModules();
} 


int hook_modules_construct(EventLoop * eventLoop_p){
    listNode * current;
    Module * value = NULL;
    int module_return;
    ntLogging(LOG_DEBUG,"module construct");

    listIter * modules_iter = listGetIterator(modules, AL_START_HEAD);
    while(NULL != (current = listNext(modules_iter))) {
        value = (Module *)current->value; 

        step_cyc :
        if (value != NULL && value->_construct != NULL){
            module_return = value->_construct(value, eventLoop_p); 
            if (module_return == STEP_FORWARD){
                continue; 
            } else if (module_return  == STEP_OVER){
                return STEP_OVER; 
            } else if (module_return == STEP_REWIND){
                //TOCSY  be careful about cyc 
                listRewind(modules, modules_iter); 
                continue;
            } else if (module_return = STEP_CYC){
                goto step_cyc ;
            }
        }
    }
}

int hook_modules_destruct(){
    listNode * current;
    Module * value = NULL;
    int module_return;
    ntLogging(LOG_DEBUG,"module destruct");

    listIter * modules_iter = listGetIterator(modules, AL_START_HEAD);
    while(NULL != (current = listNext(modules_iter))) {
        value = (Module *)current->value; 

        step_cyc :
        if (value != NULL && value->_construct != NULL){
            module_return = value->_destruct(value->module_context); 
            if (module_return == STEP_FORWARD){
                continue; 
            } else if (module_return  == STEP_OVER){
                return STEP_OVER; 
            } else if (module_return == STEP_REWIND){
                //TOCSY  be careful about cyc 
                listRewind(modules, modules_iter); 
                continue;
            } else if (module_return = STEP_CYC){
                goto step_cyc ;
            }
        }
    }
}


int hook_modules_accept(Client * client_p){
    listNode * current;
    Module * value = NULL;
    int module_return;
    ntLogging(LOG_DEBUG,"module accept");

    listIter * modules_iter = listGetIterator(modules, AL_START_HEAD);
    while(NULL != (current = listNext(modules_iter))) {
        value = (Module *)current->value; 

        step_cyc :
        if (value != NULL && value->_construct != NULL){
            module_return = value->_accept(value->module_context, client_p); 
            if (module_return == STEP_FORWARD){
                continue; 
            } else if (module_return  == STEP_OVER){
                return STEP_OVER; 
            } else if (module_return == STEP_REWIND){
                //TOCSY  be careful about cyc 
                listRewind(modules, modules_iter); 
                continue;
            } else if (module_return = STEP_CYC){
                goto step_cyc ;
            }
        }
    }
}


int hook_modules_do_read(Client * client_p){
    listNode * current;
    Module * value = NULL;
    int module_return;
    ntLogging(LOG_DEBUG,"module do_read");

    listIter * modules_iter = listGetIterator(modules, AL_START_HEAD);
    while(NULL != (current = listNext(modules_iter))) {
        value = (Module *)current->value; 

        step_cyc :
        if (value != NULL && value->_construct != NULL){
            module_return = value->_do_read(value->module_context, client_p); 
            if (module_return == STEP_FORWARD){
                continue; 
            } else if (module_return  == STEP_OVER){
                return STEP_OVER; 
            } else if (module_return == STEP_REWIND){
                //TOCSY  be careful about cyc 
                listRewind(modules, modules_iter); 
                continue;
            } else if (module_return = STEP_CYC){
                goto step_cyc ;
            }
        }
    }
}

int hook_modules_do(Client * client_p){
    listNode * current;
    Module * value = NULL;
    int module_return;
    ntLogging(LOG_DEBUG,"module do");

    listIter * modules_iter = listGetIterator(modules, AL_START_HEAD);
    while(NULL != (current = listNext(modules_iter))) {
        value = (Module *)current->value; 

        step_cyc :
        if (value != NULL && value->_construct != NULL){
            module_return = value->_do(value->module_context, client_p); 
            if (module_return == STEP_FORWARD){
                continue; 
            } else if (module_return  == STEP_OVER){
                return STEP_OVER; 
            } else if (module_return == STEP_REWIND){
                //TOCSY  be careful about cyc 
                listRewind(modules, modules_iter); 
                continue;
            } else if (module_return = STEP_CYC){
                goto step_cyc ;
            }
        }
    }
}


int hook_modules_do_write(Client * client_p){
    listNode * current;
    Module * value = NULL;
    int module_return;
    ntLogging(LOG_DEBUG,"module do write");

    listIter * modules_iter = listGetIterator(modules, AL_START_HEAD);
    while(NULL != (current = listNext(modules_iter))) {
        value = (Module *)current->value; 

        step_cyc :
        if (value != NULL && value->_construct != NULL){
            module_return = value->_do_write(value->module_context, client_p); 
            if (module_return == STEP_FORWARD){
                continue; 
            } else if (module_return  == STEP_OVER){
                return STEP_OVER; 
            } else if (module_return == STEP_REWIND){
                //TOCSY  be careful about cyc 
                listRewind(modules, modules_iter); 
                continue;
            } else if (module_return = STEP_CYC){
                goto step_cyc ;
            }
        }
    }
}


int hook_modules_done(Client * client_p){
    listNode * current;
    Module * value = NULL;
    int module_return;
    ntLogging(LOG_DEBUG,"module done");

    listIter * modules_iter = listGetIterator(modules, AL_START_HEAD);
    while(NULL != (current = listNext(modules_iter))) {
        value = (Module *)current->value; 

        step_cyc :
        if (value != NULL && value->_construct != NULL){
            module_return = value->_done(value->module_context, client_p); 
            if (module_return == STEP_FORWARD){
                continue; 
            } else if (module_return  == STEP_OVER){
                return STEP_OVER; 
            } else if (module_return == STEP_REWIND){
                //TOCSY  be careful about cyc 
                listRewind(modules, modules_iter); 
                continue;
            } else if (module_return = STEP_CYC){
                goto step_cyc ;
            }
        }
    }
}
