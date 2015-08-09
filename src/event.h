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

#ifndef EVENT_H

#define EVENT_H
#include <sys/time.h>
#include <sys/types.h>

#define EVENT_OK 0
#define EVENT_ERR -1 


#define IO_NONE 0
#define IO_READABLE 1
#define IO_WRITABLE 2

#define EVENT_IO_EVENTS 1
#define EVENT_TIME_EVENTS 2
#define EVENT_ALL_EVENTS (EVENT_IO_EVENTS | EVENT_TIME_EVENTS) 
#define EVENT_DONT_WAIT 4 

#define EVENT_TIME_NOT_CYC 1

struct eventLoop;

typedef void eventCallback(struct eventLoop*, int, void *, int );
typedef int   timeCallback(struct eventLoop *, int , void *);
typedef void timeFinalCallback(struct eventLoop*, void *);
typedef void beforeLoopCallback(struct eventLoop *);

typedef struct ioEvent {
    int mask;
    eventCallback *readCallback;
    eventCallback *writeCallback;
    void * clientData;
} IoEvent;

typedef struct waitingEvent{
    int fd;
    int mask;

}WaitingEvent ;

typedef struct timeEvent{
    long long  id; 

    long trigger_sec;
    long trigger_ms;     

    timeCallback *timeCallback;  //fired at trigger time
    timeFinalCallback *timeFinalCallback; // the last action 

    void *clientData;  //your data

    struct timeEvent * next; // list
}  TimeEvent;

typedef struct eventLoop {
    int maxfd; //the max fd 
    int volumn_size;  // the  eventLoop volumn ;
    long long timeEventNextId; // the time event id , id ++
    time_t lastTime;  //the last triggered time 

    IoEvent *events;  //this array will be seeked by fd 
    WaitingEvent *waiting_events;  // collect all the fired events
    TimeEvent * timeEventHead ; 
    int is_stop;
    void *apidata; 

    beforeLoopCallback* beforeLoop;
} EventLoop;

EventLoop * ntCreateEventLoop(int volumn_size);
int  delEventLoop(EventLoop * eventLoop_p);
int eventStop(EventLoop * eventLoop_p);
int createIoEvent(EventLoop * eventLoop_p, int fd, int mask,eventCallback *callback , void * clientData);
int delIoEvent(EventLoop * eventLoop_p, int fd, int mask);
int getIoEvent(EventLoop * eventLoop_p, int fd);
void getTime(long *sec, long *milsec);
void addMilsec2Now(long long milsec, long *sec, long *ms);
long long  createTimeEvent( EventLoop *eventLoop_p, long long millsec, timeCallback proc,void * clientData, timeFinalCallback finalProc );
int delTimeEvent(EventLoop * eventLoop_p, long long  id);
TimeEvent *  getNearestTime(EventLoop * eventLoop_p);
int processTimeEvent(EventLoop * eventLoop_p);
int processEvents(EventLoop * eventLoop_p, int flags);
int eventMain(EventLoop * eventLoop_p );
char * getApiName(void);
void setBeforeSleepCallback(EventLoop *eventLoop_p, beforeLoopCallback * beforeLoopProcess);


#endif /* end of include guard: EVENT_H */
