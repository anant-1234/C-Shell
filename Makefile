shell : \
			builtin.o \
			ls.o \
			pinfo.o \
			prompt.o \
			command_parsing.o \
			Main.o \
			process_handling.o \

		gcc -g -o myshell $^

clean:
	@rm -f *.o myshell


builtin.o :  builtin.c headerfile.h
	gcc -g -c -w builtin.c

ls.o :  ls.c headerfile.h
	gcc -g -c -w ls.c

pinfo.o :  pinfo.c headerfile.h
	gcc -g -c -w pinfo.c

command_parsing.o :  command_parsing.c headerfile.h
	gcc -g -c -w command_parsing.c

Main.o :  Main.c headerfile.h
	gcc -g -c -w Main.c

process_handling.o :  process_handling.c headerfile.h
	gcc -g -c -w process_handling.c

prompt.o : prompt.c headerfile.h
	gcc -g -c -w prompt.c