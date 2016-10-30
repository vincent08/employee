COM_H = ../../include/emp_comm.h /usr/include/mysql/mysql.h
FILES = emp_db.c emp_db.h ../user/emp_user.h $(COM_H)
emp_db.o: $(FILES)
	gcc -c emp_db.c

clean:
	rm emp_db.o