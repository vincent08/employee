//
// Created by 杨刚 on 10/28/16.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

#include "../../include/emp_comm.h"
#include "../cfg/emp_conf_file.h"
#include "emp_log.h"


// 应该加锁, 防止发生踩地址
int logger(en_log_level iLogLevel, char * str)
{
    // 新建日志节点
    st_log_item * pstNewLog = (st_log_item *)malloc(sizeof(st_log_item));

    if (pstNewLog == NULL)
    {
        printf("memory run out!\n");
        return RET_FAIL;

    } else {
        memset(pstNewLog, 0 , sizeof(st_log_item));
        pstNewLog->pstLogNext = NULL;
        strncpy(pstNewLog->logStr, str, strlen(str));
        time(&(pstNewLog->stLogTimeVal));
        pstNewLog->iLogItemLevel = iLogLevel;

        if (stLogAdmin.pstLogItemHead == NULL)  // 链表为空
        {
            stLogAdmin.pstLogItemHead = pstNewLog;
            stLogAdmin.pstLogItemTail = pstNewLog;
            stLogAdmin.iLogCount++;
            return RET_SUCC;
        } else {    // 链表非空
            stLogAdmin.pstLogItemTail->pstLogNext = pstNewLog;
            stLogAdmin.pstLogItemTail = stLogAdmin.pstLogItemTail->pstLogNext;
            stLogAdmin.pstLogItemTail->pstLogNext = NULL;
            stLogAdmin.iLogCount++;
            return RET_SUCC;
        }
    }
}

int write_log_item(st_log_item * pstLogItem, FILE *fp)
{
    int count = 1;
    const char cEnd[]="\r\n";

    char szLogPrefix[100] = {0};
    char szLevel[4][10] = {"[SYSTEM]", "[ERROR]", "[WARNING]", "[INFO]"};

    // 打印允许级别的日志
    if(pstLogItem->iLogItemLevel <= stLogAdmin.iLogLevel) {
        sprintf(szLogPrefix, "%s %s", szLevel[pstLogItem->iLogItemLevel],
                ctime(&pstLogItem->stLogTimeVal));

        if (strlen(szLogPrefix) > 1)
            szLogPrefix[strlen(szLogPrefix) - 1] = ':';  // 替换掉换行符
        if (strlen(szLogPrefix) < 99)
            szLogPrefix[strlen(szLogPrefix)] = ' ';

        if (count != fwrite(szLogPrefix, strlen(szLogPrefix), count, fp)) {
            printf("write error.\n");
            return RET_FAIL;
        }

        if (count != fwrite(pstLogItem->logStr, strlen(pstLogItem->logStr), count, fp)) {
            printf("write error.\n");
            return RET_FAIL;
        }

        if (count != fwrite(cEnd, strlen(cEnd), count, fp)) {
            printf("write error.\n");
            return RET_FAIL;
        }
    }
    //printf("write success.\n");
    return RET_SUCC;
}

int processLogThread()
{
    st_log_item * pTmp = NULL;
    FILE *fp = NULL;
    struct stat logFileState;
    time_t nowTime;
    struct tm stNowTime, stLastModified;
    char command[50] = {0};

    while (1)
    {
        if(stLogAdmin.iLogCount > 0)
        {
            // 读取链表并写日志
            if(! stLogAdmin.pstLogItemHead)
            {
                stLogAdmin.iLogCount = 0;
            } else{

                // 检查文件最后修改时间,若与当前时间不在一天,则将其重命名归档. stat函数在文件不存在时返回-1
                if(0==stat(DEFAULT_LOG_FILE, &logFileState)) {

                    memcpy((void *) &stLastModified, (void *) localtime(&logFileState.st_mtime),
                           sizeof(stLastModified));
                    time(&nowTime);
                    memcpy((void *) &stNowTime, (void *) localtime(&nowTime), sizeof(stNowTime));
                    if ((stLastModified.tm_year != stNowTime.tm_year) ||
                        (stLastModified.tm_yday != stNowTime.tm_yday)) {   // 不是同一天产生, 则重命名
                        sprintf(command, "%s %s %s-%d-%d-%d", "mv", DEFAULT_LOG_FILE, DEFAULT_LOG_FILE,
                                1900 + stLastModified.tm_year,
                                1 + stLastModified.tm_mon, stLastModified.tm_mday);
                        printf("run command: %s\n", command);
                        system(command);
                        memset(command, 0, strlen(command));
                    }
                }

                fp = fopen(DEFAULT_LOG_FILE, "a+");
                if(fp == NULL)
                {
                    printf("open file error. %d %s\n", errno, strerror(errno));
                    return RET_FAIL;
                }

                while (stLogAdmin.pstLogItemHead)
                {
                    //printf("LogCount: %d.\n", stLogAdmin.iLogCount);
                    write_log_item(stLogAdmin.pstLogItemHead, fp);
                    pTmp = stLogAdmin.pstLogItemHead;
                    stLogAdmin.pstLogItemHead = stLogAdmin.pstLogItemHead -> pstLogNext;
                    stLogAdmin.iLogCount --;
                    free(pTmp);
                }
                stLogAdmin.pstLogItemTail = NULL;
                stLogAdmin.iLogCount = 0;
                fclose(fp);
                fp = NULL;
            }
        }
        usleep(THREAD_SLEEP_TIME);
    }
    return RET_FAIL;
}


int initLogModule()
{
    // 从配置文件读, 并检查是否在0到3之间
    // int iLog = read_int_from_conf

    int iLevel = 0;
    if(0 != read_int_from_conf("LogLevel", &iLevel))
    {
        logger(ERROR_LOG, "read LogLevel conf error.");
        return RET_FAIL;
    }
    stLogAdmin.iLogLevel = iLevel;
    // printf("LogLevel: %d\n", stLogAdmin.iLogLevel);

    int i=0;
    st_log_item * pstLogTmp = NULL;

    stLogAdmin.iLogCount = 0;
    stLogAdmin.pstLogItemHead = NULL;
    stLogAdmin.pstLogItemTail = NULL;

    // 启动工作线程

    int iRet = 0;
    pthread_t processLogThreadId;

    iRet = pthread_create(&processLogThreadId, NULL, (
            void*) processLogThread, NULL);
    if(iRet != 0)
    {
        printf("thread start failed.\n");
        return RET_FAIL;
    }

    logger(INFO_LOG, "system start.");
    return RET_SUCC;
}