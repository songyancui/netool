/*
 *worker thread 
 * author : songyancui@gmail.com
 */

#ifndef __SGCONFIG_H
#define __SGCONFIG_H
typedef struct server_config{
        int        host;   /* it must to be ip */
        int  port;    /* it must to be  htons  */
        char  type[64];
        int master_count;
        int worker_count;
} Server_config ;

/*
 *API
 */

extern Server_config g_server_config;
 
int sgConfigInit(char * path);
int sgConfigDestroy();

#endif /*end __SGCONFIG_H  */
