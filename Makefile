all: program

exe:
	mkdir -p exe

exe/main.o: src/main.c | exe
	gcc -c -o exe/main.o src/main.c

exe/server_client.o: src/server_client.c | exe
	gcc -c -o exe/server_client.o src/server_client.c

exe/thread.o: src/thread.c | exe
	gcc -c -o exe/thread.o src/thread.c

exe/database.o: src/database.c | exe
	gcc -c -o exe/database.o src/database.c

FILEO = exe/main.o exe/server_client.o exe/thread.o exe/database.o

program: $(FILEO)
	gcc -o main $(FILEO)

clean:
	rm -f main exe/*.o