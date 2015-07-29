/*
 * config 
 * author: songyancui@gmail.com
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "third/cJSON/cJSON.h"
#include "log.h"


#include "ntconfig.h"


Server_config g_server_config;
cJSON *  get_config_from_file(char * path){
	FILE *f;
	long len;
	char * data; 
	cJSON * config_json;

	if( NULL == path){
		ntLogging(LOG_FATAL, "%s","The config path is empty");
		exit(-1);
	}
	
	if((access(path ,4))==-1){
		ntLogging(LOG_FATAL, "The config file is not exist, the path is %s", path);
		exit(-1);
	}

	f = fopen(path, "rb");
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	data = (char *)malloc(len + 1);
	fread(data, 1, len, f);
	fclose(f);
	config_json = cJSON_Parse(data);		
	
	return config_json;
	
}

int ntConfigInit(char * path){
	int ip_num;
	cJSON * config ;
	config = get_config_from_file(path);
	char *value;	
	int  valueInt;	

	if(!config){
		ntLogging(LOG_FATAL, "Could not parse config file %s", path);
	}
	
	value = cJSON_GetObjectItem(config, "host")->valuestring;
	if(value){
		inet_pton(AF_INET,value,&ip_num);
		g_server_config.host = ip_num;
		ntLogging(LOG_DEBUG, "%s","load the config[ip]");
	}else{
		inet_pton(AF_INET,"127.0.0.1",&ip_num);
		g_server_config.host = ip_num;
		ntLogging(LOG_WARNING, "%s","load the  default config[ip]");
	}
    value = NULL;

	valueInt = cJSON_GetObjectItem (config, "port")->valueint;
	if(valueInt){
		g_server_config.port = valueInt;
		ntLogging(LOG_DEBUG, "%s","load the config[port]");
	}else{
		g_server_config.port = 2013;
		ntLogging(LOG_DEBUG, "%s","load the default config[port]");
		ntLogging(LOG_WARNING, "%s","There is no port in the config file");
	}
	valueInt = -1;
	
	valueInt = cJSON_GetObjectItem (config, "master_count")->valueint;
	if(valueInt){
		g_server_config.master_count = valueInt;
		ntLogging(LOG_DEBUG, "%s","load the config[master_count]");
	}else {
		g_server_config.worker_count = 2;
		ntLogging(LOG_DEBUG, "%s","load the default config[master_count]");
		ntLogging(LOG_WARNING, "%s","There is no master_count in the config file");
	}
	valueInt = -1;

	valueInt = cJSON_GetObjectItem (config, "worker_count")->valueint;
	if(valueInt){
		g_server_config.worker_count = valueInt;
		ntLogging(LOG_DEBUG, "%s","load the config[work_count]");
	} else {
		g_server_config.worker_count = 4;
		ntLogging(LOG_DEBUG, "%s","load the default config[work_count]");
		ntLogging(LOG_WARNING, "%s","There is no worker_count in the config file");
	}
	valueInt = -1;

	value = cJSON_GetObjectItem(config, "mode")->valuestring;
	if(value){
        strcpy(g_server_config.mode, value );
		ntLogging(LOG_DEBUG, "%s","load the config[mode]");
    }else{
        strcpy(g_server_config.mode,"single");
        ntLogging(LOG_WARNING,"there is no mode , default single" );
    }
    
	return 1;
	
}

int ntConfigDestroy(){
	ntLogging(LOG_DEBUG,"%s","config destructed \n");
	//TODO
}

