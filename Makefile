SOURCES = src/main.c \
			src/server.c

all: 
	gcc -Wall -Wextra -o main $(SOURCES) -lraylib -lm -ldl -pthread
	./main

server_client:
	gcc -Wall -Wextra -o server_client src/server_client.c
	./server_client

clean:
	rm -f main server_client