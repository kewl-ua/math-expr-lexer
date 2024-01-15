DEPENDENCIES=lex.o hash-table.o
FLAGS=

compile: lib
	gcc -o main main.c ${DEPENDENCIES} ${FLAGS}

lib:
	gcc -c lex.c hash-table.c

run:
	main.exe

clean:
	del ${DEPENDENCIES} main.exe