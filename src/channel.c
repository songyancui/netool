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
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#include "log.h"
#include "channel.h"
#include "mm.h" 


Channel *create_channel(){
	Channel * channel_ptr;
	channel_ptr  = ntmalloc(sizeof(Channel));
	ntmemset(channel_ptr,0,sizeof(Channel));
	if(socketpair(AF_UNIX, SOCK_STREAM,0 ,channel_ptr->fd)<0){
		ntLogging(LOG_FATAL,"%s","create channel failed");
	}
	ntLogging(LOG_DEBUG,"%s","create channel successful");
	return channel_ptr;		
}

int	send_msg_channel(int fd,Inner_msg * msg){
		if(fd < 0) {
			ntLogging(LOG_WARNING,"%s","There is no fd to send");
		}
    
        if (msg == NULL) {
            ntLogging(LOG_WARNING,"the message empty in send_msg_channel");    
            return -1;
        }

		int write_len;
		ntLogging(LOG_DEBUG,"%s %d","writing the msg",msg->command);
		
		write_len = write(fd, msg, sizeof(Inner_msg));
		if(write_len != sizeof(Inner_msg)){  //TODO  care interupting  and block
			ntLogging(LOG_WARNING,"%s write_len is  %d"," writing message failed  ",write_len);
		}
			ntLogging(LOG_DEBUG,"%s size: %d","send complete",write_len);
	}

int	receive_msg_channel(int fd,Inner_msg *msg){
		Inner_msg *inner_msg_ptr ;
		inner_msg_ptr = msg;	
		int receive_msg_len;
        
		if(fd < 0){
			ntLogging(LOG_WARNING,"%s","fd < 0 , can not receive message");
			return  -1;	
		}

		receive_msg_len = read(fd ,inner_msg_ptr, sizeof(Inner_msg));
			
		if(receive_msg_len < 0){
				ntLogging(LOG_DEBUG,"%s errno:%d ","receive err",errno);
				return -1;
		}
		ntLogging(LOG_DEBUG,"%s %d %d","receive the mess",inner_msg_ptr->command,sizeof(Inner_msg));
		return 1;
	}

int	close_channel(Channel * channel_ptr){
		if(channel_ptr == NULL){
			ntLogging(LOG_WARNING,"%s","The channel has been closed");
			return ;
        }
		close(channel_ptr->fd[0]);
		close(channel_ptr->fd[1]);
        free(channel_ptr);
        channel_ptr = NULL;
}

Inner_msg * create_inner_msg(){
   Inner_msg * inner_msg_p;
   inner_msg_p = ntmalloc(sizeof(Inner_msg));    

   if (inner_msg_p == NULL) {
        ntLogging(LOG_WARNING,"create inner msg failed" );     
        return NULL;
   }
   return inner_msg_p;
}

void inner_msg_init(Inner_msg * inner_msg_p){
    if (inner_msg_p == NULL) {
        ntLogging(LOG_WARNING,"the inner message is empty");
    }
    inner_msg_p->status = -1;
    inner_msg_p->command = -1;
}

#ifdef TEST
#include "test.h"
#include "log.h"

int main(int argc, const char *argv[])
{
    ntLogInit (LOG_NOTICE, NULL);
    Channel * channel_p;
    channel_p = create_channel();
    ntassert_not_NULL(channel_p, "channel create");
   
    Inner_msg * inner_msg_p ;
    Inner_msg *receive_inner_msg_p;
    inner_msg_p = create_inner_msg();
    receive_inner_msg_p = create_inner_msg();

    ntassert_not_NULL(inner_msg_p, "inner msg create"); 
    ntassert_not_NULL(receive_inner_msg_p, "inner msg create"); 

    inner_msg_init(inner_msg_p); 
    inner_msg_init(receive_inner_msg_p); 

    inner_msg_p->status = 4;  //just for testing
    inner_msg_p->command = 4; //just fors testing

    
    send_msg_channel(channel_p->fd[0], inner_msg_p); 
    receive_msg_channel(channel_p->fd[1], receive_inner_msg_p);
    
    return 0;
}

#endif

