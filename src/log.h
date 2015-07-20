/*
 *
 *shotgun log 
 *@author : songyancui@gmail.com
 *time : 2013-02-27 1:40:00
 */


#ifndef __LOG_H__
#define __LOG_H__


#define LOG_FATAL   4
#define LOG_WARNING 3
#define LOG_NOTICE  2
#define LOG_INFO    1
#define LOG_DEBUG   0 

#define LOG_RAW (1<<10)  /*  print log without time */

/*
 * API
 */

void    ntLogInit(int low_level, char *path);
void    ntLogRaw(int level, const char *msg);
void    ntLogging(int level, const char *fmt, ...);


#endif  /* __LOG_H__ */ 
