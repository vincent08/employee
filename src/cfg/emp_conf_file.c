//
// Created by 杨刚 on 10/27/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../../include/emp_comm.h"
#include "emp_conf_file.h"


// read a key from a line
// return: RET_SUCC or RET_FAIL
S32 read_line(char* psHead, char *pszKey, char *pszValue)
{
    char * pTemp = psHead;
    int iFlag = 0;
    int iKeyLen = strlen(pszKey);

    if (*pTemp == '\n' || *pTemp == '\r' || *pTemp == '\0' || *pTemp == '#' || *pTemp == '[')
        return RET_FAIL;


    // parse Key
    while (*pTemp != '\n' && *pTemp != '\r' && *pTemp != '\0')
    {
        if(*pTemp == ' ')
        {
            pTemp ++;
            continue;
        }
        if(*pTemp == pszKey[iFlag])
        {
            pTemp ++;
            iFlag ++;
            if(iFlag == iKeyLen) break;
        } else
        {
            return -1;
        }
    }

    // parse "="
    while (*pTemp != '\n' && *pTemp != '\r' && *pTemp != '\0')
    {
        if(*pTemp == ' ')
        {
            pTemp ++;
            continue;
        } else if (*pTemp == '=')
        {
            pTemp ++;
            break;
        } else{
            return -2;
        }
    }

    // parse Value
    iFlag = 0;
    while (*pTemp != '\n' && *pTemp != '\r' && *pTemp != '\0')
    {
        if(*pTemp == ' ')
        {
            pTemp ++;
            continue;
        } else{
            pszValue[iFlag] = *pTemp;
            iFlag ++;
            pTemp ++;
        }
    }

    pszValue[iFlag] = '\0';
    // printf("%s: %s", pszKey, pszValue);
    return RET_SUCC;

}

S32 read_from_file(const char* fp, char *pszKey, char *pszValue)
{
    FILE * pFile = NULL;
    pFile =  fopen(fp, "r");
    if(pFile == NULL)
        printf("error opening file.\n");

    char buf[1024] = {0};
    int iCount = 0;
    while (1) {
        memset(buf, 0, 1024);
        if (fgets(buf, 1024, pFile) != NULL) {

            //printf("scanning line %d: %s\n", ++iCount, buf);

            if(RET_SUCC == read_line(buf, pszKey, pszValue)) {
                fclose(pFile);
                return RET_SUCC;
            }
        } else
        {
            if (feof(pFile)) //检测到文件结束
            {
                // printf("EOF. \n");
                fclose(pFile);
                break;
            } else
            {
                printf("reading file error, max line length: 1023. \n");
                fclose(pFile);
                break;
            }
        }

    }
    return RET_FAIL;
}


S32 read_int_from_conf(char *pszKey, int *piValue)
{
    char pszValue[100]={0};
    read_from_file(CONF_FILE_PATH, pszKey, pszValue);
    if (strlen(pszValue) > 0)
    {
        *piValue = atoi(pszValue);   // 越界问题、符号问题等
        return RET_SUCC;
    } else{
        return RET_FAIL;
    }
}


S32 read_str_from_conf(char *pszKey, char *pszValue)
{
    read_from_file(CONF_FILE_PATH, pszKey, pszValue);
    if (strlen(pszValue) > 0)
    {
        return RET_SUCC;
    } else{
        return RET_FAIL;
    }
}