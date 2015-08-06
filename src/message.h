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
#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE_OK 0
#define MESSAGE_ERR -1

typedef enum{
    MSG_STAT_EMPTY = 0,  
    MSG_STAT_PROCESSING,
} msg_stat_t;

typedef struct inner_message{
    int data_len;
    void * data; 
}Inner_message ;

typedef struct outer_message {
    msg_stat_t status; // mark data status 
    int data_len;
    Outer_message_cb outer_message_callback;
} Outer_message;

typedef struct outer_message_cb{
    void * (* _outer_msg_parser)(struct outer_message);     
    void * (* _outer_msg_del)(struct outer_message);
//    void * selfData;
} Outer_message_cb;


#endif /* end of include guard: MESSAGE_H */
