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
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include "log.h" 

/** FILE DESC  option  **/
int ntFdNonBlock(int fd){
    if (fd < 0) {
        ntLogging(LOG_WARNING,"fd is invalid value" );     
        return NET_ERR;
    }
    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        ntLogging(LOG_WARNING,"fcntl F_GETFL error");  
        return NET_ERR;
    }

    if (fcntl(fd, F_SETFL, flags|O_NONBLOCK) == -1) {
        ntLogging(LOG_WARNING,"fcntl F_SETFL error");    
        return NET_ERR;
    }

    return NET_OK;
}

/** FILE DESC  END **/

/** application  **/
int ntResolve(char * host, char *ipstr){
    struct sockaddr_in sa;

    if (inet_aton(host, &sa.sin_addr) == 0) {
        //host is domian
        struct hostent *he;     

        he = gethostbyname(host);
        if (he == NULL) {
            ntLogging(LOG_WARNING,"resolve failed ");    
            return NET_ERR;

        }
        memcpy(&sa.sin_addr, he->h_addr, sizeof(struct in_addr));
    }
    strcpy(ipstr, inet_ntoa(sa.sin_addr)); 
    return NET_OK;
}


/** application END **/

/** SOCKET OPT **/

//set the socket send buffer
int ntSocketSetSendBuffer(int fd, int buffsize){
    if (setsocketopt(fd, SOL_SOCKET, SO_SNDBUF, &buffsize, sizeof(buffsize)) == -1) {
         ntLogging(LOG_WARNING,"setsocketopt(SO_SNDBUF) failed");
         return NET_ERR;
    }
    return NET_OK;
    
}

int ntSockSetReuseAddr(int fd){
    if (fd < 0) {
        ntLogging(LOG_WARNING,"socketsetReuseAddr failed");
        return NET_ERR;
    }

    int flag = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) == -1) {
        ntLogging(LOG_WARNING,"setsockeopt(SO_REUSEADDR) failed" );    
        return NET_ERR;
    }
    return NET_OK;
}

int ntCreateNetStreamSocket(){
    int s;        
    if ((s = socket(AF_INET, SOCKET_STREAM), 0) == -1){
        ntLogging(LOG_WARNING,"create socket(AF_INET,SOCKET_STREAM) failed" );    
        return NET_ERR;
    }
    return s;

}

int ntCreateUnixStreamSocket(){
    int s;        
    if ((s = socket(AF_LOCAL, SOCKET_STREAM), 0) == -1){
        ntLogging(LOG_WARNING,"create socket(AF_INET,SOCKET_STREAM) failed" );    
        return NET_ERR;
    }
    return s;

}

int ntCreateStreamSocket(int domain) {
    int s;
    switch(domain) {
        //unix domain stream socket 
        case AF_LOCAL:
            s = ntCreateUnixStreamSocket(); 
            break;
        //net stream socket 
        default :
            s = ntCreateNetStreamSocket();
    }
    if (s == NET_ERR) {
        ntLogging(LOG_WARNING,"create stream socket failed");
    }
    return s;
}

int ntListen(int s, struct sockaddr *sa, socklen_t len){
    if (sa == NULL) {
        ntLogging(LOG_WARNING,"ntListen failed, param sa is NULL");
        return NET_ERR;
    }

    if (len < 0 || s < 0){
        ntLogging(LOG_WARNING,"ntListen failed, param len  or socketfd is smaller than 0" );
        return NET_ERR;
    }

    if (bind(s, sa, len) == -1){
        ntLogging(LOG_WARNING,"bind socket failed:%s" ,strerror(errno));
        close(s);
        return NET_ERR;
    }


    if (listen(s, LISTEN_BACKLOG) == -1){
        ntLogging(LOG_WARNING,"listen failed: %s", strerror(errno) );
        close(s);
        return NET_ERR;
    }

    return NET_OK;
}

int ntTcpServer(int port, char *bindaddr){
    int s;
    struct sockaddr_in sa;

    if ((s = ntCreateStreamSocket(AF_INET)) == NET_ERR){
        return NET_ERR;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bindaddr && inet_aton(bindaddr, &sa.sin_addr) == 0){
        ntLogging(LOG_WARNING,"listen socekt bind addr failed,bindadr invalid");
        close(s);
        return NET_ERR;
    }
    if (ntListen(s, (struct sockaddr*)&sa, sizeof(sa)) ==  NET_ERR) {
        return NET_ERR;
    }
    return s;
}

int ntUnixListen(char *path, mode_t perm){
    int s;
    struct sockaddr_un su;

    if (path == NULL) {
        ntLogging(LOG_WARNING,"UnixListen param path is NULL");
        return NET_ERR;
    }

    if ((s = ntCreateStreamSocket(AF_LOCAL)) == NET_ERR) {
        return NET_ERR;
    }

    memset(&su,0, sizeof(su));
    su.sun_family = AF_LOCAL;
    strncpy(su.sun_path, path, sizeof(su.sun_path)-1);

    if (ntListen(s,(struct sockaddr *)&su, sizeof(su)) == NET_ERR){
        ntLogging(LOG_WARNING,"unix socket listen failed, %s",strerror(errno) );
        return NET_ERR; 
    }
    
    if (perm){
        chmod(su.sun_path, perm);
    }

    return s;
}

int ntGenericAccept(int s, struct sockaddr *sa, socklen_t *len){
    int client_fd;
    while(1) {
        client_fd = accept(s, sa, len) {
           if (client_fd == -1) {
                if (errno == EINTR) {
                    continue;
                }else{
                    ntLogging(LOG_WARNING,"accept error:%s", strerror(errno));
                    return NET_ERR;
                }

           }
            break;
        }
    }
    return client_fd;
}

int ntTcpAccept(int s, char *ip, int *port){
    int fd;
    struct sockaddr_in sa;
    socklen_t salen = sizeof(sa);
    if ((fd = ntGenericAccept(s, (struct sockaddr *) &sa, &salen)) == NET_ERR) {
        return NET_ERR;
    }

    if (ip) strcpy(ip, inet_itoa(sa.sin_addr));
    if (port) *port = ntohs(sa.sin_port);
    return  fd;
}

int ntUnixAccept(int s){
    int fd;
    struct sockaddr_un su;
    socklen_t salen = sizeof(su);

    if ((fd == ntGenericAccept(s, (struct sockaddr *)&su, &salen)) == NET_ERR){
        return NET_ERR;
    }

    return fd;
}

/** SOCKET OPT END **/

/** TCP option **/
int ntTcpNoDelay(int fd){
    int value ;
    if (fd < -1){
        ntLogging(LOG_WARNING,"fd is invalid value", );    
        return NET_ERR;
    } 
    
    value = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value)) == -1){
         ntLogging(LOG_WARNING,"setsockopt(TCP_NODELAY) failed");
         return NET_ERR;
    }

    return NET_OK;
}

#define NET_CONNECT_NONE 0
#define NET_CONNECT_NONBLOCK 1
int ntNetGenericConnect(char * addr, int port, int flags) {
    if (port < 0) {
        ntLogging(LOG_NOTICE,"ntTcpConnect param :port < 0" );
        port  = 80;
    }

    if (addr == NULL) {
         ntLogging(LOG_WARNING,"ntTcpConnect addr == NULL");
         addr = "127.0.0.1";
    }

    int s;
    struct sockaddr_in sa;

    if ((s = ntCreateStreamSocket(AF_INET)) == NET_ERR ) {
        ntLogging(LOG_WARNING,"ntCreateStreamSocket failed");
        return NET_ERR;
    }
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if (inet_aton(addr, &sa.sin_addr) == 0) {
        struct hostent *he;

        he = gethostbyname(addr);
        if (he == NULL) {
             ntLogging(LOG_WARNING,"can not resolve" );
             return NET_ERR;
        }
        memcpy(sa.sin_addr, he->h_addr);
    }

    if (flags & NET_CONNECT_NONBLOCK) {
        if (ntFdNonBlock(s) != NET_OK) {
            return NET_ERR; 
        }
    }

    if (connect(s, (struct sockaddr * )&sa, sizeof(sa) == -1)){
        if (errno == EINPROGRESS && flags& NET_CONNECT_NONBLOCK){
            return s;
        }
        ntLogging(LOG_WARNING,"connect %",strerror(errno) );
        close(s);
        return NET_ERR;
    }
    return s;
}

int ntTcpConnect(char * addr, int port ){
    return ntNetGenericConnect(addr, port, NET_CONNECT_NONE);
}

int ntTcpNonBlockConnect(char * addr, int port){
    return ntNetGenericConnect(addr, port, NET_CONNECT_NONBLOCK);
}

int ntUnixGenericConnect(char * path, int flags){
    if (path == NULL){
        ntLogging(LOG_WARNING,"ntUnixGenericConnect->path is NULL "); return NET_ERR;
    }

    if (port < 0){
        ntLogging(LOG_WARNING,"ntUnixGenericConnect->port is NULL");
        return NET_ERR;
    }

    struct sockaddr_un su;

    if ((s = ntCreateStreamSocket(AF_LOCAL)) == NET_ERR){
        return NET_ERR;
    }
    su.sun_family = AF_LOCAL;
    strncpy(su.sun_path, path,sizeof(su.sun_path)-1);

    if (flags & NET_CONNECT_NONBLOCK) {
        if (NET_ERR == ntFdNonBlock(s)){
            ntLogging(LOG_WARNING,"unix connecting set non-block failed" );
            return NET_ERR;
        }
    } 

    if (connect(s, (struct sockaddr*)&su, sizeof(su)) == -1){
        if (errno == EINPROGRESS&& flags&NET_CONNECT_NONBLOCK) {
            return NET_OK;
        }
        ntLogging(LOG_WARNING,"unix connect failed %s",strerror(errno) );
        return NET_ERR;
    }
    return s;

}

int ntUnixNonBlockConnect(char *path){
    return ntUnixGenericConnect(path, NET_CONNECT_NONBLOCK);
}

int ntUnixConnect(char *path){
    return ntUnixGenericConnect(path, NET_CONNECT_NONE);
}

int ntPeerToString(int fd, char *ip, int *port) {
    struct sockaddr_in sa;
    socklen_t salen = sizeof(sa);

    if (getpeername(fd,(struct sockaddr*)&sa,&salen) == -1) {
        *port = 0;
        ip[0] = '?';
        ip[1] = '\0';
        return -1;
    }
    if (ip) strcpy(ip,inet_ntoa(sa.sin_addr));
    if (port) *port = ntohs(sa.sin_port);
    return 0;
}

int ntSockName(int fd, char *ip, int *port) {
    struct sockaddr_in sa;
    socklen_t salen = sizeof(sa);

    if (getsockname(fd,(struct sockaddr*)&sa,&salen) == -1) {
        *port = 0;
        ip[0] = '?';
        ip[1] = '\0';
        return -1;
    }
    if (ip) strcpy(ip,inet_ntoa(sa.sin_addr));
    if (port) *port = ntohs(sa.sin_port);
    return 0;
}


/** TCP opiotn END **/

#ifdef TEST 
#include "test.h" 

int main(int argc, const char *argv[])
{
    
    return 0;
}
#endif
