/** * 
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
#include "../master.h"
#include "../worker.h"
#include "../modules/modules.h"
#include "../event.h"
#include "../net.h"
#include "../ntconfig.h"
#include "../client.h"

#include "mode.h"

void writeCallback(struct eventLoop * eventLoop_p, int fd, void * clientData, int mask){
    ntLogging(LOG_DEBUG,"in write callback" );
    Client * cp = (Client *)clientData;

    HOOK_MODULES_WRITING(cp);
    ntwriteEasyByCount(fd,"OK", 2);

    mask |= IO_READABLE|IO_WRITABLE;
    delIoEvent(eventLoop_p, fd, mask);
    HOOK_MODULES_DONE(cp);
}

void readCallback(struct eventLoop * eventLoop_p, int fd, void * clientData, int mask){
    char * send_msg = "master_send_msg";
    char recv_str[15];
    int recv_totlen;
    Client * cp = (Client *)clientData;

    recv_totlen = ntreadEasyByCount(fd,recv_str, 15 );
    HOOK_MODULES_READING(cp);
    HOOK_MODULES_PROCESSING(cp);

    mask |= IO_WRITABLE;
    createIoEvent(eventLoop_p, fd, mask, writeCallback, cp);
}

void acceptCallback(EventLoop * eventLoop_p, int listen_fd, void * clientData, int mask){
    ntLogging(LOG_DEBUG,"accept callback");
    int client_fd = 0;
    int client_mask = 0;
    Client *cp;
    
    client_fd = ntTcpAccept(listen_fd,NULL, NULL) ;

    if (client_fd != NET_ERR){
        cp = createClient(client_fd); 
        HOOK_MODULES_ACCEPT(cp);
        client_mask |=IO_READABLE;
        if (EVENT_ERR == createIoEvent(eventLoop_p, client_fd, client_mask, readCallback,cp)){
            ntLogging(LOG_WARNING,"create client fd IO_READABLE failed client_fd:%d",client_fd );  
        }
    }
}

Worker * createSingleWorker(){
    Worker * wp = NULL;
     
    wp = createWorker(1, NULL);
    if (wp == NULL){
        ntLogging(LOG_FATAL,"createSingleWorker failed");
        return NULL;
    }
    return wp;
}

void * single_mode_init(Mode * mode_p){
    ntLogging(LOG_DEBUG,"single mode init");

    Worker * wp = NULL;
    wp = createSingleWorker();
    loadAllModules();
    HOOK_MODULES_CONSTRUCT(wp->eventLoop_p);

    mode_p->mode_data = (Worker *)wp;
    mode_p->mode_data_del = delWorker; 
    return MODE_OK;
}

int single_mode_end(Mode * mode_p){
    if (mode_p != NULL && mode_p->mode_data != NULL && mode_p->mode_data_del != NULL){
        mode_p->mode_data_del(mode_p->mode_data);
    }
    ntLogging(LOG_DEBUG,"single mode end" );
}

void single_mode_process(Mode *mode_p){
    ntLogging(LOG_DEBUG,"single mode process");
    int listen_fd= -1; 
    int port;
    port  = g_server_config.port; 
    int host ; //TOCSY host have to be filtered
    listen_fd = ntTcpServer(port,NULL);
    if (NET_ERR == ntSockSetReuseAddr(listen_fd)) {
         ntLogging(LOG_FATAL,"canot reuse socket fd" );
    }

    int mask=0;
    EventLoop * eventLoop_p;
    Worker * wp;
    wp = mode_p->mode_data;
    
    eventLoop_p = wp->eventLoop_p;

    mask |= IO_READABLE;
    if (EVENT_OK != createIoEvent(eventLoop_p, listen_fd, mask, acceptCallback, NULL)){
         ntLogging(LOG_FATAL,"add listen fd failed" );
    }

   if (EVENT_OK == eventMain(eventLoop_p)){
        ntLogging(LOG_DEBUG,"eventMain start" );
   }    

}

Mode mode_single = {
    "single",
    single_mode_init,
    single_mode_end,
    single_mode_process,
    NULL,
    NULL,
};
