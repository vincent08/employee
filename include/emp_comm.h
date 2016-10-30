//
// Created by 杨刚 on 10/27/16.
//

#ifndef EMPLOYEE_EMP_COMM_H
#define EMPLOYEE_EMP_COMM_H

typedef unsigned char U8;
typedef char S8;
typedef unsigned short U16;
typedef short S16;
typedef unsigned int U32;
typedef int S32;

typedef unsigned char _BCD;   // BCD code

typedef void VOID;

#define TRUE 1
#define FALSE 0

#define RET_SUCC 0
#define RET_FAIL -1

#define   GONG_HAO_CHANG_DU   40    /*工号所占字节长度*/
#define   XING_MING_CHANG_DU     40   /*姓名所占字节长度*/
#define   MING_LING_CHANG_DU               128   /*命令所占字节长度*/
#define  MAX_STAFF_NUMBER 1000

//性别枚举
enum XING_BIE_ENUM
{
    XING_BIE_NAN = 0,  /*男*/
    XING_BIE_NV,       /*女*/
    XING_BIE_HUN_DA,   /*混搭*/
    XING_BIE_MAX       /*不支持，max表示最大值得意思*/
};


/*员工信息结构*/
typedef struct st_employee
{
    char szcNumber[GONG_HAO_CHANG_DU];  /*工号*/
    char szcName[XING_MING_CHANG_DU];   /*姓名*/
    char cSex;                          /*性别，0表示男，1表示女，2表示男女混搭*/
    int iAge;                          /*年龄*/
    struct st_employee *pNextEm;         /*下一个员工的内存地址*/
}employee_st;



typedef struct global_variable
{
    int SAVE_FLAG; //保存文件动作的标志位，1表示需要执行写入磁盘的函数，0表示不需要

    int CURRENT_STAFF_NUM;  // 当前员工总数

    /*定义员工链表的头，因为开始没有员工，所以链表是空的,
注意全局变量系统会自动初始化为零，但我一般还是会初始化一下*/
    employee_st *g_pstHeadEmployee;

    employee_st *pst_HeadAssist;

}st_global;

#endif //EMPLOYEE_EMP_COMM_H
