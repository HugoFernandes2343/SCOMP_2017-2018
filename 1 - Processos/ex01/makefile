#variaveis

MAIN = cod
OTHER = 
EXEC = ex01


${EXEC}: ${MAIN}.o ${OTHER}.o
	gcc -Wall -g ${MAIN}.o ${OTHER}.o -o ${EXEC}

${MAIN}.o: ${MAIN}.c ${OTHER}.h
	gcc -Wall -g -c ${MAIN}.c

${OTHER}.o: ${OTHER}.s
	gcc -Wall -g -c ${OTHER}.s

run: ${EXEC}
	./${EXEC}
clean:
	rm *.o ${EXEC}
	
debug: ${EXEC}
	gdb -tui ${EXEC}
