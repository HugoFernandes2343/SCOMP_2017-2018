#variaveis

all: reader writer

reader: reader.c
	gcc -Wall -g reader.c -o reader -lrt

writer: writer.c
	gcc -Wall -g writer.c -o writer -lrt
	

run: all
	./writer
clean:
	rm writer reader
	

