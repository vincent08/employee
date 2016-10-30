SUBDIRS = cfg log
OBJS = employee.o cfg/emp_conf_file.o log/emp_log.o db/emp_db.o user/emp_user.o
H_FILES = ../include/emp_comm.h cfg/emp_conf_file.h log/emp_log.h db/emp_db.h user/emp_user.h

emp: $(OBJS)
	for dir in $(SUBDIRS); do make -C $$dir; done
	gcc -o ../bin/emp -lpthread  -I /usr/include/mysql/ -L /usr/lib/mysql/ -lmysqlclient -g $(OBJS)

employee.o:
	gcc -c employee.c $(H_FILES)

clean:
	for dir in $(SUBDIRS); do make -C $$dir clean; done
	rm employee.o