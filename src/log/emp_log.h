//
// Created by 杨刚 on 10/28/16.
//

#ifndef EMPLOYEE_EMP_LOG_H
#define EMPLOYEE_EMP_LOG_H

#include <sys/time.h>

#define SYSTEM_LOG 0
#define ERROR_LOG 1
#define WARNING_LOG 2
#define INFO_LOG 3

#define LOG_FILE_DIR "../log/"
#define DEFAULT_LOG_FILE "../log/emp.log"

#define MAX_LOG_STR_LENGTH 1024

#define THREAD_SLEEP_TIME 1000*1000  // us

// 日志级别枚举类型
typedef enum log_level_en
{
    sysLevel=0,
    errLevel,
    warnLevel,
    infoLevel
}en_log_level;


// 日志结构
typedef struct log_item_st
{
    en_log_level iLogItemLevel;
    time_t stLogTimeVal;
    char logStr[MAX_LOG_STR_LENGTH];
    struct log_item_st * pstLogNext;

}st_log_item;


// 日志管理结构
typedef struct log_admin_st
{
    en_log_level iLogLevel;
    int iLogCount;
    st_log_item * pstLogItemHead;
    st_log_item * pstLogItemTail;
}st_log_admin;

st_log_admin stLogAdmin;



extern int initLogModule();
extern int logger(en_log_level iLogLevel, char *str);


#endif //EMPLOYEE_EMP_LOG_H
