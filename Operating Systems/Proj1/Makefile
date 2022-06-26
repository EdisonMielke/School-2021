all : pseudo-shell

    
pseudo-shell: main.o command.o string_parser.o
	gcc -o pseudo-shell main.o command.o string_parser.o


main.o: main.c
	gcc -c main.c

string-parser.o: string_parser.c string_parser.h
	gcc -c string_parser.c

command.o: command.c command.h
	gcc -c command.c

clean: 
	rm -f core *.o pseudo-shell
