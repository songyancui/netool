/*
 * use for Loging
 * author: songyancui@gmail.com
 * time : 2013-2-27 18:06:00
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

#include "log.h"

#define MAX_LOGMSG_LEN 1024 /* maximum length of the syslog message  */

int g_logging_min_level = LOG_NOTICE;
char *g_logging_path = NULL;

void sgLogInit(int low_level, char * path){
    g_logging_path = path;
    g_logging_min_level = low_level;  
}

void sgLogging(int level, const char *fmt,...){
    va_list ap;

    char msg[MAX_LOGMSG_LEN];
    
    if((level&0xff) <g_logging_min_level) return ;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);
    return sgLogRaw(level, msg);
}

void sgLogRaw(int level, const char *msg){
    const char *c = ".-*#@";

    FILE *fp;
    char buf[64];
    char *logging_path = g_logging_path;
    int rawmode = level&LOG_RAW;
    
    fp =(logging_path==NULL)?stdout:fopen(logging_path, "a");

    if (!fp) return;
    if (rawmode) {
        fprintf(fp, "%s \n", msg);
    } else {
        int off;
        struct timeval tv;
        
        gettimeofday(&tv,NULL);
        struct tm dummy_time;
        memcpy(&dummy_time, localtime(&tv.tv_sec), sizeof(struct tm));
        off = strftime(buf, sizeof(buf), "%d %b %H:%M:%S.",&dummy_time);
        snprintf(buf+off, sizeof(buf)-off, "%03d",(int)tv.tv_usec/1000);
        fprintf(fp,"[%d] %s %c %s \n", (int)getpid(),buf, c[level], msg) ;

    }
    fflush(fp);
    if(logging_path) fclose(fp);
}
