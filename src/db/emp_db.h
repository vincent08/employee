//
// Created by 杨刚 on 10/28/16.
//

#ifndef EMPLOYEE_DB_H
#define EMPLOYEE_DB_H

#ifdef linux
#include "/usr/include/mysql/mysql.h"
#else
#include "mysql.h"
#endif

#include "../cfg/emp_conf_file.h"
#include "../log/emp_log.h"
#include "../user/emp_user.h"


#define DB_THREAD_SLEEP_TIME 1000*1000  // us

#define SQL_CREATE_DATABASE  "CREATE TABLE basic_info(id VARCHAR(40) PRIMARY KEY, name VARCHAR(40) NOT NULL, sex VARCHAR(1) NOT NULL, age int NOT NULL)"


typedef struct db_admin_st
{
    char szDbName[50];
    char szDbUserName[50];
    char szDbUserPassword[50];
    char szDbServerIP[50];
    int iDbServerPort;
    int iDbMaxHandle;
} st_db_admin;

st_db_admin stDbAdmin;


typedef enum operation
{
    op_add = 0,
    op_delete,
    op_modify,
    op_select,
}op_type;

// 待执行的sql操作的消息队列
typedef struct msg_node_st
{
    op_type op;
    employee_st empNode;
    struct msg_node_st *pNext;
}st_msg_node;

typedef struct db_global_st
{
    int iConnectState;
    int iMaxEmployeeCount;
    MYSQL* pMySqlConn;
    st_msg_node * pstMsgHead;
    st_msg_node * pstMsgTail;
} st_db_global;

st_db_global stDbGlobal;


extern int initDbModule();

extern int disconnect_db();

extern int query_all_records();

extern int addMsgNode(op_type op, employee_st empNode);

#endif //EMPLOYEE_DB_H
