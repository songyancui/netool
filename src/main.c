#include "./ntconfig.h"

int main(int argc, char ** argv){
		
char *conf_file_path = "./../conf/main.json";	

	ntConfigInit(conf_file_path);
	return 0;
}

