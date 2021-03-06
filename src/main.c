#include <stdio.h>
#include <stdlib.h>

#include "./log.h"
#include "./ntconfig.h"
#include "./master.h"
#include "modes/mode.h"
#include "modules/modules.h"

int main(int argc, char ** argv){
		
    char *conf_file_path = "./../conf/main.json";	
    Master *master_item;
    Mode * mode_p;
    dict * Modules;

    ntLogInit(LOG_WARNING, NULL);
    //ntLogInit(LOG_DEBUG, NULL);
	ntConfigInit(conf_file_path);
    ntLogging(LOG_DEBUG, " load the host: %d", g_server_config.host);
    ntLogging(LOG_DEBUG, " load the port: %d", g_server_config.port);
    ntLogging(LOG_DEBUG, " worker_count: %d", g_server_config.worker_count);
    ntLogging(LOG_DEBUG, " master_count: %d", g_server_config.master_count);
    ntLogging(LOG_DEBUG, " mode: %s", g_server_config.mode);
    int i=0;
    for(i ; g_server_config.modules[i] != NULL; i++){
        ntLogging(LOG_DEBUG,"modules:%s",g_server_config.modules[i]);
    }


    /** mode  start**/
    mode_p = createMode();
    ntLogging(LOG_DEBUG,"crete mode successful");
    initMode(mode_p);

    runMode(mode_p);
    /** mode  end **/




    master_item = createMaster();
    if (master_item == NULL) {
        ntLogging(LOG_FATAL," master create failed");
       exit (-1);  
    }
    
      
	return 0;
}

