

all: writer reader ex14


ex14: main.o
	gcc -Wall -g main.o  -o ex14 -lpthread -lrt
main.o: main.c
	gcc -Wall -g -c main.c


reader:	reader.o
	gcc -Wall -g reader.o -o reader -lpthread -lrt
reader.o: reader.c
	gcc -Wall -g -c reader.c

writer: writer.o
	gcc -Wall -g writer.o -o writer -lpthread -lrt
writer.o: writer.c
	gcc -Wall -g -c writer.c


run: ex14
	./ex14

clean:
	rm *.o ex14
	
