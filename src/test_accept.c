#include <stdio.h>
#include <stdlib.h>
#include "event.h"
#include "net.h"
#include "log.h"
#include "io.h"
#include "test.h"

void writeCallback(struct eventLoop * eventLoop_p, int fd, void * clientData, int mask){
    ntwriteEasyByCount(fd,"OK", 2);
    mask |= IO_READABLE|IO_WRITABLE;
    delIoEvent(eventLoop_p, fd, mask);
    long long timeId = -1;
}

void readCallback(struct eventLoop * eventLoop_p, int fd, void * clientData, int mask){
    int recv_totlen;
    char recv_str[1024];

    recv_totlen = read(fd,recv_str, 1024);
    //mask |= IO_WRITABLE;
   // createIoEvent(eventLoop_p, fd, mask, writeCallback, NULL);
    delIoEvent(eventLoop_p, fd, mask);
    close(fd);
}

void acceptCallback(struct eventLoop * eventLoop_p, int fd, void * clientData, int mask){
    int client_fd= 0;
    client_fd = ntUnixAccept(fd);
    //delIoEvent(eventLoop_p, client_fd, mask);
    //close(client_fd);
    int client_mask =0;
    client_mask |= IO_READABLE;
    createIoEvent(eventLoop_p, client_fd, client_mask, readCallback, NULL);    
}

int main(int argc, char ** argv){
    ntLogInit(LOG_DEBUG, NULL); 
    int port = 2014;
    int master_listen_net_fd=-1;
    master_listen_net_fd = ntTcpServer(port,NULL); 
    ntSockSetReuseAddr(master_listen_net_fd);
    EventLoop *eventLoop_p = ntCreateEventLoop(1024); 
    int mask = 0;
    mask |= IO_READABLE;
    createIoEvent(eventLoop_p, master_listen_net_fd,mask,acceptCallback,NULL);
    eventMain(eventLoop_p); 
    delEventLoop(eventLoop_p);
}

