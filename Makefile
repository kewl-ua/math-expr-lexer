DEPENDENCIES=lex.o hash-table.o

compile: lib
	gcc -o main main.c ${DEPENDENCIES}

lib:
	gcc -c lex.c hash-table.c

run:
	main.exe

clean:
	del ${DEPENDENCIES} main.exe