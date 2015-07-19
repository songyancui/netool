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


#ifndef __MASTER_H
#define __MASTER_H

#include <sys/types.h>
#include "channel.h"

typedef struct master{
	pid_t pid;
	int master_id;
	int status;
	Channel *channel;   //use for communicating
} Master ;

/*
 *API
 */

 /*  Master status  */
#define MASTER_STATUS_PREPARE 0
#define MASTER_STATUS_RUNNING 1
#define MASTER_STATUS_STOPING 2
#define MASTER_STATUS_STOPED  3
#define MASTER_STATUS_CHANGE_CONFIG 4
/* Controller process Orders the Worders within Commands below*/
#define MASTER_COMMANDED_STOP 0
#define MASTER_COMMANDED_START 1
#define MASTER_COMMANDED_CHANGE_CONFIG 2

 /*
  * API  
  */
 
	Master * master_create();
	int master_run();
	int master_stop(Master *m, int soft);


	
#endif /*end __MASTER_H  */
