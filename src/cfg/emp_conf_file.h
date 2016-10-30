//
// Created by 杨刚 on 10/27/16.
//

#ifndef EMPLOYEE_EMP_CONF_FILE_H
#define EMPLOYEE_EMP_CONF_FILE_H

#define CONF_FILE_PATH "../employee.conf"

#include "../../include/emp_comm.h"

extern int read_int_from_conf(char *pszKey, int *piValue);
extern int read_str_from_conf(char *pszKey, char *pszValue);

#endif //EMPLOYEE_EMP_CONF_FILE_H
