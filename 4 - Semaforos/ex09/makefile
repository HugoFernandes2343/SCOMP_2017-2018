#variaveis

MAIN = main
EXEC = ex09


${EXEC}: ${MAIN}.o
	gcc -Wall -g ${MAIN}.o  -o ${EXEC} -lpthread 

${MAIN}.o: ${MAIN}.c
	gcc -Wall -g -c ${MAIN}.c

run: ${EXEC}
	./${EXEC}
clean:
	rm *.o ${EXEC}
	
debug: ${EXEC}
	gdb -tui ${EXEC}
