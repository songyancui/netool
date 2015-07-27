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
#include <string.h>
#include <sys/epoll.h>
#include "mm.h"
#include "log.h"
#include "event.h"

typedef struct apiContext{
    int epfd;
    struct epoll_event * events;
} ApiContext;


int apiCreate(EventLoop * eventLoop_p){
    ApiContext * ac= ntmalloc(sizeof (ApiContext));  
    
    if (ac == NULL){
        ntLogging(LOG_FATAL,"create epoll event error" );
        
        return -1;
    }

    ac->events = (struct epoll_event *)ntmalloc(sizeof(struct epoll_event) * eventLoop_p->volumn_size);

    if (ac->events == NULL){
        ntLogging(LOG_FATAL,"create epoll event error, malloc events" );
        ntfree(ac);
        return -1;
    }
    
    ac->epfd = epoll_create(1024); 
    if (ac->epfd == -1){
        ntfree(ac->events);
        ntfree(ac);
        ntLogging(LOG_FATAL,"epoll_create failed" );
        return -1;
    }

    eventLoop_p->apidata = ac;
    return 0;
}

void apiFree(EventLoop *eventLoop_p){
    ApiContext * ac = eventLoop_p->apidata; 
    ntfree(ac->events);
    close(ac-> epfd);
    ntfree(ac);
}

int apiAddEvent(EventLoop * eventLoop_p, int fd, int mask){
    ApiContext * ac = eventLoop_p->apidata; 
    struct epoll_event ee;

    int op = eventLoop_p->events[fd].mask == IO_NONE? EPOLL_CTL_ADD:EPOLL_CTL_MOD;

    ee.events = 0;

    mask |= eventLoop_p->events[fd].mask;

    if (mask & IO_READABLE) ee.events |= EPOLLIN;
    if (mask & IO_WRITABLE) ee.events |= EPOLLOUT;

    ee.data.u64 = 0;
    ee.data.fd = fd;

    if (epoll_ctl(ac->epfd, op, fd, &ee) ==  -1) return -1;
        return 0;
}

void apiDelEvent(EventLoop *eventLoop_p, int fd, int delmask){
    ApiContext * ac = eventLoop_p->apidata;

    struct epoll_event ee;
    int op = (eventLoop_p->events[fd].mask & (~delmask)) == IO_NONE ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

    ee.events = 0;

    int mask = eventLoop_p->events[fd].mask & (~delmask); 

    if (mask & IO_READABLE) ee.events |= EPOLLIN;
    if (mask & IO_WRITABLE) ee.events |= EPOLLOUT;

    ee.data.u64 = 0;
    ee.data.fd = fd;
    epoll_ctl(ac->epfd, op, fd, &ee);

}

int apiPoll(EventLoop * eventLoop_p, struct timeval *tvp){
    ApiContext * ac = eventLoop_p->apidata;

    int retval, numevents =0;
    long long time_msc;
    time_msc = tvp? tvp->tv_sec * 1000 + tvp->tv_usec/1000: -1; 
    retval = epoll_wait(ac->epfd, ac->events, eventLoop_p->volumn_size, time_msc);

    if (retval > 0){
        int j;
        for (j = 0; j < retval;  j++){
            int mask;
            struct epoll_event *ee = ac->events+ j;
            if (ee->events & EPOLLIN) mask |= IO_READABLE;
            if (ee->events & EPOLLOUT) mask |= IO_WRITABLE;
            if (ee->events & EPOLLERR) mask |= IO_WRITABLE;
            if (ee->events & EPOLLHUP) mask |= IO_WRITABLE;

            eventLoop_p->waiting_events[j].fd = ee->data.fd;
            eventLoop_p->waiting_events[j].mask = mask;
        }
    }
    return retval;
}


char *apiName(void ){
    return "epoll";
}
