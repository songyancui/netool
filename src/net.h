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

#ifndef NET_H
#define NET_H
#include "net.h"

#define NET_OK 0 
#define NET_ERR -1
#define LISTEN_BACKLOG 1024

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/stat.h>

int ntFdNonBlock(int );
int ntResolve(char * , char *);
int ntSocketSetSendBuffer(int , int );
int ntSockSetReuseAddr(int );
int ntCreateNetStreamSocket();
int ntCreateUnixStreamSocket();
int ntCreateStreamSocket(int );
int ntListen(int , struct sockaddr *, socklen_t);
int ntTcpServer(int, char *);
int ntUnixServer(char *, mode_t);
int ntGenericAccept(int , struct sockaddr *, socklen_t *);
int ntTcpAccept(int , char *, int *);
int ntUnixAccept(int );
int ntTcpNoDelay(int);
int ntNetGenericConnect(char*, int , int);
int ntTcpConnect(char *, int);
int ntTcpNonBlockConnect(char *, int );
int ntUnixGenericConnect(char *, int );
int ntUnixNonBlockConnect(char *);
int ntUnixConnect(char *);
int ntPeerToString(int, char *, int *);
int ntSockName(int, char *, int *);


#endif /* end of include guard: NET_H */

