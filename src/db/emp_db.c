//
// Created by 杨刚 on 10/28/16.
//

#include <stdio.h>
#include <stdlib.h>

#include "emp_db.h"

int connect_db()
{
    stDbGlobal.pMySqlConn = NULL;
    stDbGlobal.pMySqlConn = mysql_init(NULL);
    if(stDbGlobal.pMySqlConn == NULL)
    {
        printf("Error %u: %s\n", mysql_errno(stDbGlobal.pMySqlConn), mysql_error(stDbGlobal.pMySqlConn));
        return RET_FAIL;
    }

    if(NULL == mysql_real_connect(stDbGlobal.pMySqlConn, stDbAdmin.szDbServerIP, stDbAdmin.szDbUserName, stDbAdmin.szDbUserPassword,
                                  stDbAdmin.szDbName, (unsigned int)stDbAdmin.iDbServerPort, NULL, 0))
    {
        printf("Error %u: %s\n", mysql_errno(stDbGlobal.pMySqlConn), mysql_error(stDbGlobal.pMySqlConn));
        return RET_FAIL;
    }

    // 进行必要的设置
    stDbGlobal.pMySqlConn->reconnect = 1; // 断线重连
    // mysql_autocommit(stDbGlobal.pMySqlConn, FALSE); // 手动提交SQL语句

    printf("database collect success.\n");
    stDbGlobal.iConnectState = 1;

    return RET_SUCC;
}


int disconnect_db()
{
    if(stDbGlobal.iConnectState == 0)
    {
        printf("Database has not been connected.\n");
        return RET_SUCC;
    }
    mysql_close(stDbGlobal.pMySqlConn);
    stDbGlobal.pMySqlConn = NULL;
    stDbGlobal.iConnectState = 0;

    printf("close db connection success.\n");
    return RET_SUCC;
}


int insert_record(employee_st empNode)
{
    char command[200] = {0};
    sprintf(command,"INSERT INTO basic_info (id, name, sex, age) VALUE ('%s', '%s', '%d', %d)", empNode.szcNumber, empNode.szcName,
             empNode.cSex, empNode.iAge);
    int iRet = 0;
    iRet = mysql_real_query(stDbGlobal.pMySqlConn, command, strlen(command));
    if(0 != iRet)
    {
        printf("ex mysql_real_query to insert failed,errno=%d\n",iRet);
        return RET_FAIL;
    }
    return RET_SUCC;
}


int delete_record(employee_st empNode)
{
    char command[200] = {0};
    sprintf(command,"DELETE FROM basic_info WHERE id='%s'", empNode.szcNumber);
    int iRet = 0;
    iRet = mysql_real_query(stDbGlobal.pMySqlConn, command, strlen(command));
    if(0 != iRet)
    {
        printf("ex mysql_real_query to delete failed,errno=%d\n",iRet);
        return RET_FAIL;
    }
    return RET_SUCC;
}


int query_all_records()
{
    char command[200] = {0};
    sprintf(command,"SELECT * FROM basic_info");
    int iRet = 0;
    iRet = mysql_real_query(stDbGlobal.pMySqlConn, command, strlen(command));
    if(0 != iRet)
    {
        printf("ex mysql_real_query to get all records failed,errno=%d\n",iRet);
        return RET_FAIL;
    }

    MYSQL_RES* pMySqlRes = mysql_store_result(stDbGlobal.pMySqlConn);

    MYSQL_ROW row;   // char ** 类型
    employee_st empNode;

    int i=0;
    while(NULL != (row = mysql_fetch_row(pMySqlRes))){
//        for (i = 0;  i < mysql_num_fields(pMySqlRes);  i ++)
//            printf ("%s ",row[i]); //this is printed like it should be.
//        printf("\n");
        memset(&empNode, 0 , sizeof(empNode));
        strcpy(empNode.szcNumber, row[0]);
        strcpy(empNode.szcName, row[1]);
        empNode.cSex = atoi(row[2]);
        empNode.iAge = atoi(row[3]);
        insert_from_mysql_row(empNode);
    }


    mysql_free_result(pMySqlRes);
    return RET_SUCC;
}


// 增加消息队列的消息
int addMsgNode(op_type op, employee_st empNode)
{
    st_msg_node *pTmp = NULL;
    pTmp = (st_msg_node *)malloc(sizeof(st_msg_node));
    if(pTmp == NULL)
    {
        printf("no enough memory.\n");
        return RET_FAIL;
    }
    memset(pTmp, 0, sizeof(st_msg_node));

    memcpy(&(pTmp->empNode), &empNode, sizeof(empNode));

    if(stDbGlobal.pstMsgHead == NULL)
    {
        stDbGlobal.pstMsgHead = pTmp;
        stDbGlobal.pstMsgTail = pTmp;
        stDbGlobal.pstMsgHead ->op = op;
        stDbGlobal.pstMsgHead ->pNext = NULL;
    } else{
        stDbGlobal.pstMsgTail = pTmp;
        pTmp->pNext = NULL;
        pTmp->op = op;
        stDbGlobal.pstMsgTail = stDbGlobal.pstMsgTail->pNext;
    }

    //printf("New node added.\n");

    return RET_SUCC;
}


// 处理消息队列的消息
int processMsgQueueThread()
{
    st_msg_node *pTmp = NULL;

    while (1)
    {
        // printf("Msg queue checked.\n");
        if(stDbGlobal.pstMsgHead != NULL){
            //printf("Valid node found!\n");
            while (stDbGlobal.pstMsgHead){
                switch(stDbGlobal.pstMsgHead->op)
                {
                    case 0:
                        insert_record(stDbGlobal.pstMsgHead->empNode);
                        break;
                    case 1:
                        delete_record(stDbGlobal.pstMsgHead->empNode);
                        break;
                    default:
                        break;
                }
                pTmp = stDbGlobal.pstMsgHead;
                stDbGlobal.pstMsgHead = stDbGlobal.pstMsgHead->pNext;
                free(pTmp);
            }
            stDbGlobal.pstMsgTail = NULL;

        }
        usleep(DB_THREAD_SLEEP_TIME);
    }
    return RET_FAIL;
}


int initDbModule()
{
    //初始化数据库模块
    int iFlag = 0;

    iFlag += read_str_from_conf("DbName", stDbAdmin.szDbName);
    iFlag += read_str_from_conf("DbUserName", stDbAdmin.szDbUserName);
    iFlag += read_str_from_conf("DbUserPassword", stDbAdmin.szDbUserPassword);
    iFlag += read_str_from_conf("DbServerIP", stDbAdmin.szDbServerIP);
    iFlag += read_int_from_conf("DbServerPort", &stDbAdmin.iDbServerPort);
    iFlag += read_int_from_conf("DbMaxHandle", &stDbAdmin.iDbMaxHandle);
    iFlag += read_int_from_conf("MaxEmployeeCount", &stDbGlobal.iMaxEmployeeCount);

    if(iFlag != 0) {
        logger(ERROR_LOG, "init database failed. please check your configuration.");
        printf("init database failed. please check your configuration.\n");
        return RET_FAIL;
    }

    connect_db();

    stDbGlobal.pstMsgHead = NULL;
    stDbGlobal.pstMsgTail = NULL;

    // 启动工作线程

    int iRet = 0;
    pthread_t processMsgQueueThreadId;

    iRet = pthread_create(&processMsgQueueThreadId, NULL, (
            void*) processMsgQueueThread, NULL);
    if(iRet != 0)
    {
        printf("thread start failed.\n");
        return RET_FAIL;
    }

    logger(INFO_LOG, "init database module success.\0");
    return RET_SUCC;
}