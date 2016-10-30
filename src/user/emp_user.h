//
// Created by 杨刚 on 10/28/16.
//

#ifndef EMPLOYEE_EMP_USER_H
#define EMPLOYEE_EMP_USER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "../../include/emp_comm.h"
#include "../cfg/emp_conf_file.h"
#include "../log/emp_log.h"
#include "../db/emp_db.h"

extern int initUserModule();

extern int insert_from_mysql_row( employee_st stEmInfor);

#endif //EMPLOYEE_EMP_USER_H
