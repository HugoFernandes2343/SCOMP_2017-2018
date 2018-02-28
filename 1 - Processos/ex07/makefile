#variaveis

MAIN = main
EXEC = ex07


${EXEC}: ${MAIN}.o
	gcc -Wall -g ${MAIN}.o  -o ${EXEC}

${MAIN}.o: ${MAIN}.c
	gcc -Wall -g -c ${MAIN}.c

run: ${EXEC}
	./${EXEC}
clean:
	rm *.o ${EXEC}
	
debug: ${EXEC}
	gdb -tui ${EXEC}
