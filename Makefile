SOURCES = src/main.c

all: 
	gcc -Wall -Wextra -o main $(SOURCES) -lraylib -lm -ldl -pthread
	./main

clean:
	rm -f main