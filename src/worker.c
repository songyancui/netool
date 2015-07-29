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

#include "log.h"
#include "mm.h"
#include "channel.h"
#include "net.h"

#include "worker.h"


Worker *  createWorker(int id, Channel * order_channel){
    Worker * wp;
    wp = ntmalloc(sizeof (Worker));
    if (wp == NULL){
        ntLogging(LOG_FATAL,"create worker failed");
        return NULL;
    }
    
    if (WORKER_ERR == initWorker(wp, id, order_channel)){
        ntLogging(LOG_FATAL,"create worker failed");
        return NULL;
    }

    return wp;
}

int initWorker(Worker *wp, int id, Channel * order_channel){
   
    if (wp == NULL){
        return WORKER_ERR; 
    }
    wp->worker_id = id;
    wp->order_channel = order_channel ;
    
    if ((wp->eventLoop_p = ntCreateEventLoop(EVENTLOOP_VOLUMN_SIZE)) == NULL){
         ntLogging(LOG_FATAL,"worker create eventLoop failed");
        return WORKER_ERR;
    }

    return WORKER_OK;

}


void delWorker(Worker * wp){
   //TOCSY 
}
