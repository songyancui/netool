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
		sgLogging(LOG_FATAL, "%s","The config path is empty");
		exit(-1);
	}
	
	if((access(path ,4))==-1){
		sgLogging(LOG_FATAL, "The config file is not exist, the path is %s", path);
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
		sgLogging(LOG_FATAL, "Could not parse config file %s", path);
	}
	
	value = cJSON_GetObjectItem(config, "host")->valuestring;
	if(value){
		inet_pton(AF_INET,value,&ip_num);
		g_server_config.host = ip_num;
		sgLogging(LOG_DEBUG, "%s","load the config[ip]");
	}else{
		inet_pton(AF_INET,"127.0.0.1",&ip_num);
		g_server_config.host = ip_num;
		sgLogging(LOG_WARNING, "%s","load the  default config[ip]");
	}

	valueInt = cJSON_GetObjectItem (config, "port")->valueint;
	if(valueInt){
		g_server_config.port = valueInt;
		sgLogging(LOG_DEBUG, "%s","load the config[port]");
	}else{
		g_server_config.port = 2013;
		sgLogging(LOG_DEBUG, "%s","load the default config[port]");
		sgLogging(LOG_WARNING, "%s","There is no port in the config file");
	}
	valueInt = -1;
	
	valueInt = cJSON_GetObjectItem (config, "master_count")->valueint;
	if(valueInt){
		g_server_config.master_count = valueInt;
		sgLogging(LOG_DEBUG, "%s","load the config[master_count]");
	}else {
		g_server_config.worker_count = 2;
		sgLogging(LOG_DEBUG, "%s","load the default config[master_count]");
		sgLogging(LOG_WARNING, "%s","There is no master_count in the config file");
	}
	valueInt = -1;

	valueInt = cJSON_GetObjectItem (config, "worker_count")->valueint;
	if(valueInt){
		g_server_config.worker_count = valueInt;
		sgLogging(LOG_DEBUG, "%s","load the config[work_count]");
	} else {
		g_server_config.worker_count = 4;
		sgLogging(LOG_DEBUG, "%s","load the default config[work_count]");
		sgLogging(LOG_WARNING, "%s","There is no worker_count in the config file");
	}
	valueInt = -1;

	return 1;
	
}

int ntConfigDestroy(){
	sgLogging(LOG_DEBUG,"%s","config destructed \n");
	//TODO
}


