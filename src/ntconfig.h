/*
 * config parser 
 * author : songyancui@gmail.com
 */

#ifndef __NTCONFIG_H
#define __NTCONFIG_H
#include "modules/modules.h"

typedef struct server_config{
        int        host;   /* it must to be ip */
        int  port;    /* it must to be  htons  */
        char  type[64];
        int master_count;
        int worker_count;
        char mode[10];
        char * modules[MAX_MODULES_SIZE];

} Server_config ;

/*
 *API
 */

extern Server_config g_server_config;
 
int ntConfigInit(char * path);
int ntConfigDestroy();

#endif /*end __NTCONFIG_H  */
