//
// Created by 杨刚 on 10/27/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "../include/emp_comm.h"
#include "cfg/emp_conf_file.h"
#include "log/emp_log.h"
#include "db/emp_db.h"
#include "user/emp_user.h"

int main() {

    int level;
    char str[100]={0};

    printf("If error occurs, please change to the directory 'source/' and execute command '../bin/emp'.\n");

    read_str_from_conf("Version", str);
    printf("Version: %s\n", str);
    memset(str, 0, 100);
    read_str_from_conf("Company", str);
    printf("Company: %s\n", str);
    memset(str, 0, 100);

    initLogModule();
    initDbModule();
    initUserModule();

    logger(INFO_LOG, "system stop.");
    disconnect_db();
    sleep(1);

    return 0;
}