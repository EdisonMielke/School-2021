all : part1 part2 iobound cpubound

cpubound: cpubound.o
	gcc -o cpubound cpubound.o

iobound: iobound.o
	gcc -o iobound iobound.o

part2: part2.o string_parser.o command.o
	gcc -o part2 part2.o string_parser.o command.o

part1: part1.o string_parser.o command.o
	gcc -o part1 part1.o string_parser.o command.o -g

part1.o: part1.c
	gcc -c part1.c

cpubound.o: cpubound.c
	gcc -c cpubound.c

iobound.o: iobound.c
	gcc -c iobound.c

string_parser.o: string_parser.c string_parser.h
	gcc -c string_parser.c

command.o: command.c command.h
	gcc -c command.c

clean:
	rm -f core *.o iobound cpubound part1 part2
