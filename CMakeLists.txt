cmake_minimum_required(VERSION 3.6)
project(employee)

INCLUDE_DIRECTORIES("/usr/local/mysql-5.7.10-osx10.9-x86_64/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    include/emp_comm.h
    src/cfg/emp_conf_file.c
    src/cfg/emp_conf_file.h
    src/db/emp_db.c
    src/db/emp_db.h
    src/log/emp_log.c
    src/log/emp_log.h
    src/user/emp_user.c
    src/user/emp_user.h
    src/employee.c
		)

add_executable(employee ${SOURCE_FILES})