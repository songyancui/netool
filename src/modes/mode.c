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

#include "mode.h"
#include "../log.h"
#include "../dict.h"
#include "mode_single.c"

Mode * createMode(){
    Mode* mode_p = NULL;

   mode_p = CURRENT_MODE_STRUCT_P;

    if (mode_p == NULL){
        ntLogging(LOG_FATAL,"malloc mode failed" );
        return NULL;
    }
    return mode_p;
}

int initMode(Mode * mode_p){
    if (mode_p == NULL){
        return MODE_ERR;
    }

    if (mode_p->_mode_init == NULL){
        return MODE_OK;
    }

    if (MODE_OK != mode_p->_mode_init(mode_p)){
        ntLogging(LOG_FATAL,"mode_init failed" );
        return MODE_ERR;
    }

    return MODE_OK;
}

int runMode(Mode * mode_p){
    if(mode_p==NULL){
        ntLogging(LOG_FATAL,"run mode failed, mode_p is NULL ");
        return MODE_ERR;
    }
    
    mode_p->_mode_process(mode_p);
    return MODE_OK;
}

int delMode(Mode *mode_p){


    if (mode_p != NULL && mode_p->_mode_end != NULL){
        mode_p->_mode_end(mode_p);
        ntfree(mode_p);
        return MODE_OK; 
    }
    if (mode_p->_mode_end == NULL){
        ntfree(mode_p);
        return MODE_OK;
    }
    return MODE_ERR;
}