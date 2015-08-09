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
 
 #include "io.h"
 #include "log.h"


int ntread(int fd, void * ptr, int length){
	int  n;
	struct iovec iov[1];
	struct msghdr msg;
	
	iov[0].iov_base = ptr;
    if (length > 0) {
	    iov[0].iov_len = length;
    }else{
	    iov[0].iov_len = MAX_RECV_LENGTH;
    }	

	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_name = NULL;
	msg.msg_namelen = 0; 
	msg.msg_iov = iov; 
    msg.msg_iovlen = 1;
	msg.msg_flags = 0;

	n = recvmsg(fd, &msg, 0);
	if(n == -1){
		ntLogging(LOG_WARNING,"%s","recvmsg failed ");
	}
		
    ntLogging(LOG_DEBUG,"%s %d messages","recvmsg", n);

	return n;
}


int ntwrite(int fd ,void * ptr, int length){
	ssize_t n;
	struct iovec iov[1];
	struct msghdr msg;
	
	iov[0].iov_base = ptr;
    if (length > 0){
	    iov[0].iov_len = length;
    }else {
	    iov[0].iov_len = sizeof(*ptr);
    }
    	
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_name =NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = iov;
	msg.msg_iovlen  =1;
	msg.msg_flags = 0;

	n = sendmsg(fd, &msg, 0);
	
	if(n == -1){
		ntLogging(LOG_WARNING,"%s, errno : %d","channel push failed", errno);
	}else{
		ntLogging(LOG_DEBUG,"%s %d messages","sendmsg", n);
	}
	return n;
}

int ntreadEasyByCount(int fd, char * buf, int count){
    int nread, totlen = 0;

    while(totlen != count) {
        nread = read(fd, buf, count - totlen);     
        if (nread == 0) return totlen;
        if (nread == -1) {
            ntLogging(LOG_WARNING,"ntreadEasyByCount error %s", strerror(errno) );
            return -1;
        }
        totlen += nread;
        buf += nread;
    }

    return totlen;
}

int ntwriteEasyByCount(int fd, char *buf, int count){
    int nwrite, totlen = 0;

    while(count != totlen) {
        nwrite = write(fd, buf, count - totlen);
        if (nwrite == 0) return totlen; //this means the send buffer is full 
        if (nwrite == -1) {
            ntLogging(LOG_WARNING,"ntwriteEasyByCount  error:%s", strerror(errno));
            return -1;
        }
        totlen += nwrite;
        buf += nwrite;
    }

    return totlen;
}
