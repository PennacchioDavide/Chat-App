SOURCES = src/main.c

all: 
	gcc -Wall -Wextra -Werror `pkg-config --cflags gtk+-3.0` -o main $(SOURCES) `pkg-config --libs gtk+-3.0`
	./main

clean:
	rm -f main