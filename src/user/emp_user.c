//
// Created by 杨刚 on 10/28/16.
//

#include "emp_user.h"

#include <sys/stat.h>
#include <fcntl.h>

st_global global_var;

/*=================================================
 功能:             初始化内存
 参数说明:         无
 返回值:
        RET_SUCC  操作成功
 特别说明:   无
======================================================*/
int init_memory()
{
    memset(&global_var, 0, sizeof(st_global));

    global_var.pst_HeadAssist = malloc(MAX_STAFF_NUMBER * sizeof(employee_st));

    if (NULL == global_var.pst_HeadAssist)
        return RET_FAIL;

    memset(global_var.pst_HeadAssist, 0, MAX_STAFF_NUMBER * sizeof(employee_st));
    employee_st * ptmp = global_var.pst_HeadAssist;

    int i=0;

    for(i=0; i<MAX_STAFF_NUMBER; i++)  // 建立结构
    {
        if(i < MAX_STAFF_NUMBER-1)
            ptmp -> pNextEm = ptmp + 1;
        else
            ptmp -> pNextEm = NULL;
        ptmp = ptmp -> pNextEm;
        //printf("%d init\n", i);
    }
    return RET_SUCC;

}

/*=================================================
 功能:             通过工号查找员工
 参数说明
     pstEm:
 返回值:        NULL 表示查找失败
                !NULL 表示查找成功
 特别说明:   无
======================================================*/

employee_st * find_employee_infor_by_number(char * pcNumber)
{
    employee_st *pstTemp = NULL;
    /*参数的合法性判断*/
    if(NULL == pcNumber)
    {
        return NULL;
    }
    pstTemp = global_var.g_pstHeadEmployee;
    while(pstTemp)
    {
        if(0 == strncmp(pcNumber,pstTemp->szcNumber,GONG_HAO_CHANG_DU))
        {
            break;
        }
        pstTemp = pstTemp->pNextEm;
    }
    return pstTemp;
}

/*=================================================
 功能:             向链表的末尾插入一个节点
 参数说明
     pstEm:     要插入的节点
 返回值:        RET_SUCC  插入成功
 特别说明:   无
======================================================*/

int insert_em()
{
    // 将辅助链表的头节点加到职员列表中
    employee_st * pstDangEm;
    pstDangEm=global_var.g_pstHeadEmployee;
    //让pstDangEm指向链表尾
    if(pstDangEm == NULL)
    {
        global_var.g_pstHeadEmployee = global_var.pst_HeadAssist;
        global_var.pst_HeadAssist = global_var.pst_HeadAssist->pNextEm;
        global_var.g_pstHeadEmployee->pNextEm = NULL;
    }

    else
    {
        while(pstDangEm->pNextEm)
        {
            pstDangEm=pstDangEm->pNextEm;
        }
        pstDangEm->pNextEm = global_var.pst_HeadAssist;
        global_var.pst_HeadAssist = global_var.pst_HeadAssist->pNextEm;
        pstDangEm->pNextEm->pNextEm = NULL;
    }


    return RET_SUCC;
}


/*=================================================
 功能:             插入一个员工信息
 参数说明:         无
 返回值:
        RET_SUCC  操作成功
 特别说明:   无
======================================================*/

int insert_em_process( )
{
    employee_st stEmInfor;

    /*请输入员工信息*/
    printf("Please input info in format of 'number name age gender':\n");

    /*第一步:让用户输入员工的工号、姓名、年龄、性别,
    输入的时候要用一个空格隔开,如0001 zhansan 25 1*/
    scanf("%s %s %d %d",stEmInfor.szcNumber,stEmInfor.szcName,
          &stEmInfor.iAge,&stEmInfor.cSex);
    /*第二步:检测用户输入数据的合法性，
    1. 检测一下用户输入的数据的正确性，这里只作一个简单的判断，判断性别的合法性*/
    if(XING_BIE_MAX <= stEmInfor.cSex)
    {
        /*您输入的性别不正确*/
        printf("error: wrong gender:%d ! ! !\n",stEmInfor.cSex);
        return RET_FAIL;
    }
    /*2. 检测一下输入的工号是否已经有人用了，因为每个人的工号是不相同的*/
    if(NULL != find_employee_infor_by_number(stEmInfor.szcNumber))
    {
        /*提醒用户:刚才所输入的工号已经被使用*/
        printf("the number has been used ! ! !\n");
        return RET_FAIL;
    }


    strcpy(global_var.pst_HeadAssist->szcNumber,stEmInfor.szcNumber);
    strcpy(global_var.pst_HeadAssist->szcName,stEmInfor.szcName);
    global_var.pst_HeadAssist->iAge = stEmInfor.iAge;
    global_var.pst_HeadAssist->cSex = stEmInfor.cSex;

    // 添加到消息队列
    addMsgNode(0, *(global_var.pst_HeadAssist));

    /*第四步:把此节点放入链表*/
    if(RET_FAIL== insert_em())
    {
        /*添加失败，一定要记得释放刚才申请的内存，否则就会产生内存泄露*/
        printf("error:add failed\n");
        return RET_FAIL;
    }
    //第五步:返回成功
    return RET_SUCC;
}


int insert_from_mysql_row( employee_st stEmInfor)
{
    strcpy(global_var.pst_HeadAssist->szcNumber,stEmInfor.szcNumber);
    strcpy(global_var.pst_HeadAssist->szcName,stEmInfor.szcName);
    global_var.pst_HeadAssist->iAge = stEmInfor.iAge;
    global_var.pst_HeadAssist->cSex = stEmInfor.cSex;

    /*第四步:把此节点放入链表*/
    if(RET_FAIL== insert_em())
    {
        /*添加失败，一定要记得释放刚才申请的内存，否则就会产生内存泄露*/
        printf("error:add failed\n");
        return RET_FAIL;
    }
    //第五步:返回成功
    return RET_SUCC;
}


/*=================================================
 功能:             删除一个值是pName的节点
 参数说明
     pName:     姓名
 返回值:
        RET_SUCC  删除成功
        RET_FAIL   删除失败
 特别说明:   无
======================================================*/
int delete_em(char *pNumber)
{
    employee_st *pstDangQian = NULL;
    employee_st *pstQian = NULL;

    /*一般来说，函数处理的第一步就是判断
    参数的合法性，特别是指针参数有问题的话，
    就会引起不可预测的错误*/
    if(NULL == pNumber)
    {
        printf("the name is NULL !!!!\r\n");
        return RET_FAIL;
    }
    /*第一步:判断一个下这个员工链表是否是空的，
    即是否有员工*/
    if(NULL == global_var.g_pstHeadEmployee)
    {
        printf("the system is null,so delete failed!!!!\r\n");
        return RET_FAIL;
    }
    /*第二步:判断链表的第一个节点是否是所要找的节点*/
    if(0== strncmp(global_var.g_pstHeadEmployee->szcNumber,pNumber,GONG_HAO_CHANG_DU))
    {
        // 向数据库模块发送删除消息
        addMsgNode(1, *(global_var.g_pstHeadEmployee));

        pstDangQian=global_var.g_pstHeadEmployee;  // 临时变量
        global_var.g_pstHeadEmployee = global_var.g_pstHeadEmployee->pNextEm;
        pstDangQian->pNextEm = global_var.pst_HeadAssist;
        global_var.pst_HeadAssist = pstDangQian;
        global_var.CURRENT_STAFF_NUM --;
        return RET_SUCC;
    }
    /*第三步:查找其它节点，并且删除该节点*/
    pstDangQian=global_var.g_pstHeadEmployee->pNextEm;
    pstQian = global_var.g_pstHeadEmployee;
    while(pstDangQian)
    {
        if(0 == strncmp(pstDangQian->szcNumber,pNumber,GONG_HAO_CHANG_DU))
        {
            // 向数据库模块发送删除消息
            addMsgNode(1, *pstDangQian);

            pstQian->pNextEm = pstDangQian->pNextEm;
            pstDangQian->pNextEm = global_var.pst_HeadAssist;
            global_var.pst_HeadAssist = pstDangQian;
            global_var.CURRENT_STAFF_NUM --;

            return RET_SUCC;
        }
        else
        {
            pstQian = pstDangQian;
            pstDangQian=pstDangQian->pNextEm;
        }
    }

    /*失败 */
    return RET_FAIL;
}


/*=================================================
 功能:             根据工号删除一个员工信息
 参数说明:         无
 返回值:
        RET_SUCC  操作成功
 特别说明:   无
======================================================*/
int delete_em_process()
{
    char szNumber[GONG_HAO_CHANG_DU];

    memset(szNumber,0,XING_MING_CHANG_DU);

    /*请输入工号*/
    printf("Please input the number:");

    /*用户输入工号,但注意，这个时候用scanf是不安全的，
    因为用户可能输入多于EMPLOYEE_NAME_LEN个的字符*/
    scanf("%s",szNumber);
    if(RET_FAIL == delete_em(szNumber))
    {
        /*删除失败，请检查你输入的工号信息*/
        printf("error: please check your number:%s!!!\n",szNumber);
        return RET_FAIL;
    }

    /*删除成功，工号是:*/
    printf("succ: the deleted number is:%s\n",szNumber);

    return RET_SUCC;
}
/*=================================================
 功能:            打印输出一个链表的所有节点
 参数说明
 返回值:
 特别说明:   无
======================================================*/

int print_em()
{
    employee_st *pstCuEm = NULL;

    pstCuEm = global_var.g_pstHeadEmployee;

    int count = 0;

    printf("----------------\n");

    while(pstCuEm)
    {
        printf("%s,%s,%d,%d\n",pstCuEm->szcNumber,pstCuEm->szcName,pstCuEm->iAge,pstCuEm->cSex);
        pstCuEm=pstCuEm->pNextEm;
        count ++;
    }
    printf("----------------\ntotal: %d items.\n", count);
    return 1;
}


int initUserModule()
{
    char szcCmd[MING_LING_CHANG_DU];

    if(RET_SUCC != init_memory())
        printf("init memory fail\n");

    // read_int_from_conf("MaxEmployeeCount", &MAX_STAFF_NUMBER);
    query_all_records();

    while(1)
    {
        /*提示用户输入命令*/

        printf("----------------\navailable command: print, del, add, exit ,quit\n");
        printf("input> ");

        /*清除一下原来命令里面的数据*/
        memset(szcCmd,0,MING_LING_CHANG_DU);
        /*等待用户输入命令，注意，这个时候用scanf是不安全的，
        属于高危函数，因为用户可能输入多于CMD_LEN个的字符，
        但本项目为了基于大家之前学的知识，暂时还用它*/
        scanf("%s",szcCmd);
        if(0==strncmp(szcCmd,"print",strlen("print")))
        {
            /*打印所有员工信息*/
            print_em();
        }
        else if(0==strncmp(szcCmd,"del",strlen("del")))
        {
            /*删除一个员工的操作*/
            delete_em_process();
            global_var.SAVE_FLAG = 1;
        }
        else if(0==strncmp(szcCmd,"add",strlen("add")))
        {
            /*增加一个员工的操作*/
            insert_em_process();
            global_var.SAVE_FLAG = 1;
        }
        else if((0==strncmp(szcCmd,"exit",strlen("exit")))||
                (0==strncmp(szcCmd,"quit",strlen("quit"))))
        {
            /*系统退出*/

            // free(global_var.pst_HeadAssist);
            // printf("writing function returns %d\n", ret);
            break;
        }
        else
        {
            /*你输入的命令暂不支持*/
            printf("bad command:%s!\r\n",szcCmd);
        }

    }
    return 0;
}

