SOURCES = src/main.c \
			src/server.c

all: 
	gcc -Wall -Wextra -o main $(SOURCES) -lraylib -lm -ldl -pthread
	./main

server:
	gcc -Wall -Wextra -o server src/server.c
	./server

clean:
	rm -f main