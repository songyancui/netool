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
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include "event.h"
#include "log.h"
#include "mm.h"
#include "net.h"

//TOCSY: have to adapte kernal

#define HAVE_EPOLL

#ifdef HAVE_EPOLL
    #include "ev_epoll.c"
#else
    #include "ev_select.c"
#endif


EventLoop * ntCreateEventLoop(int volumn_size){
    EventLoop * eventLoop_p;
    if (volumn_size < 1) {
        ntLogging(LOG_FATAL,"create EventLoop volumn is invalid");
        return NULL; 
    }

    if ((eventLoop_p = (EventLoop *)ntmalloc(sizeof(EventLoop))) == NULL) {
        ntLogging(LOG_FATAL,"malloc EventLoop failed" );
        goto error; 
    }
    

    if ((eventLoop_p->events = (IoEvent *)ntmalloc(sizeof(IoEvent) * volumn_size)) == NULL) {
        ntLogging(LOG_FATAL,"malloc EventLoop->events failed" );
        goto error; 
    }

    if ((eventLoop_p->waiting_events = (WaitingEvent  *)ntmalloc(sizeof(WaitingEvent ) * volumn_size)) == NULL) {
        ntLogging(LOG_FATAL,"malloc EventLoop->waiting_events failed" );
        goto error; 
    }


    eventLoop_p->maxfd = -1;
    eventLoop_p->volumn_size = volumn_size;

    eventLoop_p->timeEventNextId = 0;
    eventLoop_p->lastTime = time(NULL);
    eventLoop_p->timeEventHead = NULL;

    eventLoop_p->is_stop = 0;
    eventLoop_p->beforeLoop = NULL;

    if (apiCreate(eventLoop_p) == -1){
        ntLogging(LOG_FATAL,"canot create api loop " );
        goto error;
    }

    int i;
    for(i=0; i < volumn_size; i++)
        eventLoop_p->events[i].mask = IO_NONE;

    return eventLoop_p; 

    error:
        if (eventLoop_p){
            ntfree(eventLoop_p->events); 
            ntfree(eventLoop_p->waiting_events); 
            ntfree(eventLoop_p);
        }
        return NULL;

}


int  delEventLoop(EventLoop * eventLoop_p){
    apiFree(eventLoop_p);
    ntfree(eventLoop_p->events);
    ntfree(eventLoop_p->waiting_events);
    ntfree(eventLoop_p);
    return EVENT_OK;
}

int eventStop(EventLoop * eventLoop_p){
    eventLoop_p->is_stop = 1;
    return EVENT_OK;
}

int createIoEvent(EventLoop * eventLoop_p, int fd, int mask,eventCallback *callback , void * clientData){
    if (fd > eventLoop_p->volumn_size) {
        return EVENT_ERR;
    }

    IoEvent *io_event;
    io_event = &eventLoop_p->events[fd];
    if(apiAddEvent(eventLoop_p, fd, mask) == -1){
        return EVENT_ERR; 
    }
    io_event->mask |=mask;

    if(mask & IO_READABLE) io_event->readCallback = callback;
    if(mask & IO_WRITABLE) io_event->writeCallback = callback;

    io_event->clientData = clientData;

    if (fd > eventLoop_p->maxfd){
        eventLoop_p->maxfd = fd;
    }

    return EVENT_OK;
}

int delIoEvent(EventLoop * eventLoop_p, int fd, int mask) {
    if (fd > eventLoop_p->volumn_size){
        return EVENT_ERR;
    }

    IoEvent * io_event =&eventLoop_p->events[fd]; 
    io_event->mask =io_event->mask & (~mask);

    if (io_event->mask == IO_NONE && eventLoop_p->maxfd == fd){
        int j;
        for (j = fd -1; j >= 0; j--){
            if (eventLoop_p->events[j].mask != IO_NONE) 
                break;
        }
        eventLoop_p->maxfd = j;
    }
    apiDelEvent(eventLoop_p, fd, mask);

}


int getIoEvent(EventLoop * eventLoop_p, int fd){
    if (fd > eventLoop_p->volumn_size) return 0;
    return eventLoop_p->events[fd].mask;
}

void getTime(long *sec, long *milsec){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *sec = tv.tv_sec;
    *milsec = tv.tv_usec/1000;
}

void addMilsec2Now(long long milsec, long *sec, long *ms){
    long cur_sec, cur_ms, when_sec, when_ms;

    getTime(&cur_sec, &cur_ms);
    
    when_sec = cur_sec + milsec / 1000;
    when_ms = cur_ms + milsec % 1000;
    if (when_ms > 1000){ when_sec ++; when_ms -= 1000; } 
    *sec = when_sec;
    *ms = when_ms;
}


long long  createTimeEvent( EventLoop *eventLoop_p, long long millsec, timeCallback proc,void * clientData, timeFinalCallback finalProc ){
    long long id = eventLoop_p->timeEventNextId++;
    long sec, ms;
    TimeEvent  * timeEvent_p ;
    timeEvent_p = ntmalloc(sizeof (TimeEvent));
    if (timeEvent_p == NULL){
        ntLogging(LOG_WARNING,"timeevent create failed" );
        return EVENT_ERR;
    }

    addMilsec2Now(millsec, &sec, &ms); 
    timeEvent_p-> id = id;
    timeEvent_p->timeCallback = proc;
    timeEvent_p->timeFinalCallback = finalProc;
    timeEvent_p->trigger_sec = sec;
    timeEvent_p->trigger_ms = ms;
    timeEvent_p->clientData = clientData;
    
    timeEvent_p->next = eventLoop_p->timeEventHead ;
    eventLoop_p->timeEventHead = timeEvent_p;
    return id;
}

//TOCSY :o(n)
int delTimeEvent(EventLoop * eventLoop_p, long long  id){
    TimeEvent * te, *prev;
    prev = NULL;
    te = eventLoop_p->timeEventHead; 
    while(te){
        if (te->id == id){
            if (prev == NULL){
               eventLoop_p->timeEventHead = te->next; 
            }else{
                prev->next = te->next;
            }
            if (te->timeFinalCallback)
                te->timeFinalCallback(eventLoop_p, te->clientData);
            ntfree(te);
            return EVENT_OK;
        }else{
            prev = te; 
            te = te->next;
        }
    }
    return EVENT_ERR;
}

//TOCSY:O(n)  need to optimaze 
TimeEvent *  getNearestTime(EventLoop * eventLoop_p){
   TimeEvent * te = NULL, *nearest = NULL ; 

    te = eventLoop_p->timeEventHead;
    while(te){
        if (!nearest || nearest->trigger_sec > te->trigger_sec || (nearest->trigger_sec == te->trigger_sec && nearest->trigger_ms > te->trigger_ms)){
            nearest = te;
            te = te->next;
        } 
    }
    return nearest; 
}

int processTimeEvent(EventLoop * eventLoop_p){
    int processed = 0;
    TimeEvent * te;
    long long maxId;
    time_t now = time(NULL);

    
    te = eventLoop_p->timeEventHead;
    maxId = eventLoop_p->timeEventNextId-1;
    while(te) {
        long now_sec, now_ms;
        long long id;
        if (te->id > maxId)
            continue;
        getTime(&now_sec, &now_ms);
        if (te->trigger_sec < now_sec || (te->trigger_sec == now_sec && te->trigger_ms < now_ms)){
            int retval;
            id = te->id;
            retval = te->timeCallback(eventLoop_p, id, te->clientData);  
            processed ++;
            if (retval != EVENT_TIME_NOT_CYC){
                addMilsec2Now(retval, &te->trigger_sec, &te->trigger_ms) ;
            }else{
                delTimeEvent(eventLoop_p, id);     
            }
            //when process current time event , the timeEventHead could be changed
            //so change the te to  the first 

            te = eventLoop_p->timeEventHead; 
        }else{
            te = te->next;
        }
    }
    return processed;
}

/**
* flags
* EVENT_IO_EVENTS  : process the triggered io events
* EVENT_TIME_EVENTS: process time triggered events
* EVENT_ALL_EVENTS : process IO and time events
* EVENT_DONT_WAIT : the epoll timeout set -1
*/

int processEvents(EventLoop * eventLoop_p, int flags){
    int processed=0, numevents; 
    struct timeval tv, *tvp;

    if (!(flags&EVENT_IO_EVENTS)||!(flags&EVENT_TIME_EVENTS)){
        return 0;
    }

    TimeEvent * nearest = NULL;

    if ((flags& EVENT_TIME_EVENTS) && !(flags & EVENT_DONT_WAIT)){
        nearest = getNearestTime(eventLoop_p); 
    }
    if (nearest){
        long now_sec , now_ms;
        getTime(&now_sec, &now_ms);
        tvp = &tv;
        tvp->tv_sec = nearest->trigger_sec - now_sec;
         
        if (nearest->trigger_ms < now_ms) {
            tvp->tv_usec = ((nearest->trigger_ms+ 1000) - now_ms) * 1000;
            tvp->tv_sec --;
        } else{
            tvp->tv_usec = (nearest->trigger_ms - now_ms) * 1000;
        }
    
        if (tvp->tv_sec < 0) tvp->tv_sec = 0;
        if (tvp->tv_usec < 0) tvp->tv_usec = 0;
              
    } else{
        if (flags &EVENT_DONT_WAIT) {
            tv.tv_sec = tv.tv_usec = 0;
            tvp = &tv;
        }else {
            tvp = NULL;
        }
        numevents = apiPoll(eventLoop_p, tvp); 
        int j;
        for(j = 0; j < numevents; j++){
            IoEvent * ie = &eventLoop_p->events[eventLoop_p->waiting_events[j].fd];
            int mask = eventLoop_p->waiting_events[j].mask;
            int fd = eventLoop_p->waiting_events[j].fd;
            int rfired = 0;
            if (ie->mask & mask & IO_READABLE){
                rfired = 1;
                ie->readCallback(eventLoop_p, fd, ie->clientData, mask) ;
            }

            if (ie->mask & mask & IO_WRITABLE){
                if (!rfired  || ie->readCallback != ie->writeCallback){
                    ie->writeCallback(eventLoop_p, fd, ie->clientData, mask);
                }
            }
            processed ++; 
        }
    }
    if (flags & EVENT_TIME_EVENTS){
        processed += processTimeEvent(eventLoop_p);
    }
    return processed;
}


int eventMain(EventLoop * eventLoop_p ){
    eventLoop_p->is_stop = 0;
    while (!eventLoop_p->is_stop){
        if (eventLoop_p->beforeLoop !=NULL) {
            eventLoop_p->beforeLoop(eventLoop_p);
        }

        processEvents(eventLoop_p, EVENT_ALL_EVENTS);
    }

    return EVENT_OK;
}

char * getApiName(void){
    return apiName();
}

void setBeforeSleepCallback(EventLoop *eventLoop_p, beforeLoopCallback * beforeLoopProcess){
    eventLoop_p->beforeLoop = beforeLoopProcess;
}

#ifdef TEST 
#include "test.h"
#include "io.h"
#include "log.h"

int testTimeCallback(EventLoop * eventLoop_p, int id, void * clientData){
    ntassert(1, "test  timeCallback");
    return EVENT_TIME_NOT_CYC;
}

void testTimeFinalCallback(EventLoop * eventLoop_p, void * clientData){
    ntassert(1, "test timeFinalCallback"); 
    ntassert(EVENT_OK == eventStop(eventLoop_p), "test eventStop ");
}

void writeCallback(struct eventLoop * eventLoop_p, int fd, void * clientData, int mask){
    ntassert(1, "in write callback");
    ntassert(2 == ntwriteEasyByCount(fd,"OK", 2), "write resp msg completely");
    mask |= IO_READABLE|IO_WRITABLE;
    ntassert(EVENT_OK == delIoEvent(eventLoop_p, fd, mask), "delIoEvent complete");
    long long timeId = -1;
    ntassert((timeId = createTimeEvent(eventLoop_p, 1000, testTimeCallback,NULL, testTimeFinalCallback ))> -1, "test createTimeEvent ");  
        
}

void readCallback(struct eventLoop * eventLoop_p, int fd, void * clientData, int mask){
    int recv_totlen;
    char recv_str[20];

    recv_totlen = ntreadEasyByCount(fd,recv_str, 15 );
    mask |= IO_WRITABLE;
    createIoEvent(eventLoop_p, fd, mask, writeCallback, NULL);
}


void acceptCallback(struct eventLoop * eventLoop_p, int fd, void * clientData, int mask){
    ntassert(1, "in accept Callback");
    int client_fd= 0;
    client_fd = ntUnixAccept(fd);
    int client_mask;
    client_mask |= IO_READABLE;
    createIoEvent(eventLoop_p, client_fd, client_mask, readCallback, NULL);    
}


int main(int argc, const char *argv[])
{
    ntLogInit(LOG_DEBUG, NULL); 
    int master_listen_net_fd , master_accept_net_fd, client_connect_net_fd;  
    int port = TEST_PORT; 
    char recv_msg[20] ;
    char * send_msg = "master_send_msg";
    int pid;

    if ((pid = fork())> 0) {
        master_listen_net_fd = ntTcpServer(port,NULL); 
        ntSockSetReuseAddr(master_listen_net_fd);
        EventLoop *eventLoop_p = ntCreateEventLoop(1024); 
        int mask = 0;
        mask |= IO_READABLE;

        ntassert(1 ,"master accept add event");
        ntassert(EVENT_OK == createIoEvent(eventLoop_p, master_listen_net_fd,mask,acceptCallback,NULL), "createIoEvent successfully");


        ntassert(EVENT_OK == eventMain(eventLoop_p), "test eventMain"); 
        ntassert(EVENT_OK == delEventLoop(eventLoop_p), "test eventStop");
        

    } else {
        ntassert(1 ,"client test");
        sleep(1);
        client_connect_net_fd = ntTcpConnect("127.0.0.1", port);
        int send_totlen;
        int recv_totlen;
        send_totlen = ntwriteEasyByCount(client_connect_net_fd, send_msg, strlen(send_msg));
        ntassert((send_totlen == strlen(send_msg)), "client send msg completely " );
        recv_totlen = ntreadEasyByCount(client_connect_net_fd,  recv_msg, 2) ;
        ntassert(recv_totlen == 2, "client recv msg OK");
        ntassert(!strncmp(recv_msg, "OK", 2), "client recv msg correctly");
    }
    
    return 0;
}
#endif
