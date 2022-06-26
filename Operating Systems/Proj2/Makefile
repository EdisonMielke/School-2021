all : part1 part2 part3 part4

part4: part4.o string_parser.o
	gcc -o part4 part4.o string_parser.o -g

part3: part3.o string_parser.o
	gcc -o part3 part3.o string_parser.o -g

part2: part2.o string_parser.o
	gcc -o part2 part2.o string_parser.o -g

part1: part1.o string_parser.o
	gcc -o part1 part1.o string_parser.o -g

part4.o: part4.c
	gcc -c part4.c

part3.o: part3.c
	gcc -c part3.c

part2.o: part2.c
	gcc -c part2.c

part1.o: part1.c
	gcc -c part1.c

string_parser.o: string_parser.c string_parser.h
	gcc -c string_parser.c

clean:
	rm -f core *.o iobound cpubound part1 part2 part3 part4
