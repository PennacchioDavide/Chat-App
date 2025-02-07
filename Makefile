# Default target
all: program

# Ensure the exe/ directory exists
exe:
	mkdir -p exe

# Compile main.o
exe/main.o: src/main.c | exe
	gcc -Wall -Wextra -c -o exe/main.o src/main.c -lraylib -lm -ldl -pthread

# Compile server_client.o
exe/server_client.o: src/server_client.c | exe
	gcc -Wall -Wextra -c -o exe/server_client.o src/server_client.c -lraylib -lm -ldl -pthread

# Link the program
program: exe/main.o exe/server_client.o
	gcc -o main exe/main.o exe/server_client.o -lraylib -lm -ldl -pthread

# Clean up build files
clean:
	rm -f main exe/main.o exe/server_client.o