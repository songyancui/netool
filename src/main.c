#include <stdio.h>
#include <stdlib.h>

#include "./log.h"
#include "./ntconfig.h"
#include "./master.h"


int main(int argc, char ** argv){
		
char *conf_file_path = "./../conf/main.json";	
Master *master_item;


	ntConfigInit(conf_file_path);
    ntLogInit(LOG_DEBUG, NULL);
    ntLogging(LOG_DEBUG, " load the host: %d", g_server_config.host);
    ntLogging(LOG_DEBUG, " load the port: %d", g_server_config.port);
    ntLogging(LOG_DEBUG, " worker_count: %d", g_server_config.worker_count);
    ntLogging(LOG_DEBUG, " master_count: %d", g_server_config.master_count);

    master_item = master_create();
    if (master_item == NULL) {
        ntLogging(LOG_FATAL," master create failed");
       exit (-1);  
    }

      
	return 0;
}

