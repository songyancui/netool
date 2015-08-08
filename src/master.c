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
#include <unistd.h>

#include "log.h"
#include "master.h"
#include "channel.h"
#include "mm.h"


char *_master_status[5]={"MASTER_STATUS_PREPARE","MASTER_STATUS_RUNNING","MASTER_STATUS_STOPING","MASTER_STATUS_STOPED","MASTER_STATUS_CHANGE_CONFIG"};
#define MTSTATUS(n) _master_status[n]


char * _master_command[3]={"MASTER_COMMANDED_STOP","MASTER_COMMANDED_START","MASTER_COMMANDED_CHANGE_CONFIG"};
#define MC2STR(n) _master_command[n]

Master * createMaster(){
	Master *master_p;
    Channel * channel_p;
    master_p = ntmalloc(sizeof (Master));
    if (master_p == NULL){
        ntLogging(LOG_FATAL, "create master failed");        
        exit(-1);
    }
    
    master_p->status = MASTER_STATUS_PREPARE;
    ntLogging(LOG_DEBUG,"master status: %s",MTSTATUS(master_p->status));
    master_p->pid = getpid();

    channel_p = create_channel();
    if (channel_p == NULL){
        ntfree(master_p);
        return NULL;
    }
      
    
    return master_p;
}

int	runMaster(Master * m){
	ntLogging(LOG_DEBUG,"%s","master_running");
	ntLogging(LOG_DEBUG,"%s %d","master_runing",m->channel->fd[1]);
	Order_msg order_msg;
	receive_msg_channel(m->channel->fd[1],&order_msg);	
	ntLogging(LOG_DEBUG,"%s recieved %d","master_runed",order_msg.command);
}

#ifdef TEST
#include "test.h" 

int main(int argc, const char *argv[])
{
    Master * master_p = createMaster();
    ntassert_not_NULL(master_p, "master_create");


    return 0;
}
#endif
