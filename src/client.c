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
#include <errno.h>

#include "client.h"
#include "log.h"
#include "mm.h"


Client * createClient(int fd){
    if (fd < 0) {
        ntLogging(LOG_WARNING,"client fd is inavailabel");
    }
    Client * cp = ntmalloc(sizeof(Client)) ;
    if (cp == NULL){
        ntLogging(LOG_WARNING,"create Client failed" );
        return NULL;
    }
    cp->fd = fd;
    cp->recv_msg_len = 0;
    cp->recv_msg = NULL;

    cp->send_msg_len = 0;
    cp->send_msg = NULL;
    cp->have_sent_msg_len = 0;

    return cp;
}
void  delClient(Client * client_p){
    if (client_p == NULL){
        ntLogging(LOG_WARNING,"client is empty" );
        return ;    
    }
    if (client_p->fd > 0){
        close(client_p->fd);
    }
    if (client_p->recv_msg != NULL){
       ntfree(client_p->recv_msg); 
       client_p->recv_msg = NULL;
    }
    
    if (client_p->send_msg != NULL){
       ntfree(client_p->send_msg); 
       client_p->send_msg = NULL;
    }

    ntfree(client_p);
    client_p = NULL;
}

int  clientReadData(Client * client_p){
    int recv_len = -2;
    int toread_len = 0;
    if (client_p == NULL) {
        ntLogging(LOG_WARNING,"client is empty");
        return 0;
    }
    if (client_p->recv_msg == NULL) {
        client_p->recv_msg = ntmalloc(RECV_MAX_LENGTH);
        if (client_p->recv_msg == NULL){
            ntLogging(LOG_WARNING,"malloc recv msg max length failed" );
            return 0;
        }
        recv_len = ntread(client_p->fd, client_p->recv_msg, RECV_MAX_LENGTH);
        if (recv_len == -1){
            if (errno == EAGAIN){
                return -EAGAIN;
            }else{
                ntLogging(LOG_WARNING,"error occoured error:%s", strerror(errno));
                return -errno;
            }
        } else if (recv_len == 0 ){
             ntLogging(LOG_WARNING,"client close socket, so delete  client");
             delClient(client_p);
        }
        client_p->recv_msg_len = recv_len;
    } else {
        //saved old DATA
        toread_len = RECV_MAX_LENGTH - client_p->recv_msg_len;
        recv_len = ntread(client_p->fd, client_p->recv_msg+client_p->recv_msg_len , toread_len);
        if (recv_len == -1){
            if (errno == EAGAIN){
                return -EAGAIN;
            }else{
                ntLogging(LOG_WARNING,"error occoured error:%s", strerror(errno));
                return -errno;
            }
        } else if (recv_len ==0 ){
             ntLogging(LOG_WARNING,"client close socket");
        }
        client_p->recv_msg_len += recv_len;     

        if (client_p->recv_msg_len >= RECV_MAX_LENGTH){
            ntLogging(LOG_WARNING,"the recv_msg is full, so delete client");
            delClient(client_p);
        }
    }   
    return recv_len; 
}

int clientWriteData(Client * client_p){
    int send_len= -2;
    if (client_p == NULL) {
        ntLogging(LOG_WARNING, "client is empty");
        return 0;
    }    

    if (client_p->send_msg == NULL) {
       return 0; 
    }

    send_len = ntwrite(client_p->fd, client_p->send_msg + client_p->have_sent_msg_len, client_p->send_msg_len);

    if (send_len == -1){
        if (errno == EAGAIN) {
            return -EAGAIN;
        }else{
            ntLogging(LOG_WARNING,"error occoured error:%s", strerror(errno));
            return -errno;
        }
    }
    client_p->send_msg_len -= send_len;
    client_p->have_sent_msg_len += send_len;
    if (client_p->send_msg_len >0){
          ntLogging(LOG_WARNING,"writing message not complete, sent: %d, left:%d",send_len, client_p->send_msg_len);
    }
    return send_len; 
}



#ifdef TEST
int main(int argc, const char *argv[])

{
    
    return 0;
}

#endif
