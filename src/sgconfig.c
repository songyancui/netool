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
#include "./third/json-c/json.h"
#include "./third/json-c/json_util.h"
#include "log.h"

#include "sgconfig.h"


Server_config g_server_config;
int sgConfigInit(char * path){
	json_bool isExist = 0;
	json_object *obj;
	int ip_num;
	if( NULL == path){
		sgLogging(LOG_FATAL, "%s","The config path is empty");
		exit(-1);
	}
	
	if((access(path ,4))==-1){
		sgLogging(LOG_FATAL, "The config file is not exist, the path is %s", path);
		exit(-1);
	}
	json_object * config ;
	config = json_object_from_file(path);
	
	if(!config){
		sgLogging(LOG_FATAL, "Could not parse config file %s", path);
	}
	
	isExist = json_object_object_get_ex(config, "host", &obj);
	if(isExist && (json_object_get_type(obj)== json_type_string)){
		inet_pton(AF_INET,json_object_get_string(obj),&ip_num);
		g_server_config.host = ip_num;
		sgLogging(LOG_DEBUG, "%s","load the config[ip]");
	}else{
		inet_pton(AF_INET,"127.0.0.1",&ip_num);
		g_server_config.host = ip_num;
		sgLogging(LOG_WARNING, "%s","load the  default config[ip]");
	}
	
	isExist = json_object_object_get_ex(config, "port",&obj);
	if(isExist && (json_object_get_type(obj) == json_type_int)){
		g_server_config.port = json_object_get_int(obj);
		sgLogging(LOG_DEBUG, "%s","load the config[port]");
	}else{
		g_server_config.port = 2013;
		sgLogging(LOG_DEBUG, "%s","load the default config[port]");
		sgLogging(LOG_WARNING, "%s","There is no port in the config file");
	}
	
	isExist = json_object_object_get_ex(config, "master_count",&obj);
	if(isExist&& (json_object_get_type(obj) == json_type_int)){
		g_server_config.master_count = json_object_get_int(obj);
		sgLogging(LOG_DEBUG, "%s","load the config[master_count]");
	}else{
		g_server_config.worker_count = 2;
		sgLogging(LOG_DEBUG, "%s","load the default config[master_count]");
		sgLogging(LOG_WARNING, "%s","There is no master_count in the config file");
	}

	isExist = json_object_object_get_ex(config, "worker_count",&obj);
	if(isExist&& (json_object_get_type(obj) == json_type_int)){
		g_server_config.worker_count = json_object_get_int(obj);
		sgLogging(LOG_DEBUG, "%s","load the config[work_count]");
	}else{
		g_server_config.worker_count = 4;
		sgLogging(LOG_DEBUG, "%s","load the default config[work_count]");
		sgLogging(LOG_WARNING, "%s","There is no worker_count in the config file");
	}
	return 1;
	
}

int sgConfigDestroy(){
	sgLogging(LOG_DEBUG,"%s","config destructed \n");
	//TODO
}



