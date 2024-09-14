CC = gcc
CFLAGS = -Wall
TARGETS = slave.out app.out view.out

all: $(TARGETS)

SharedMemoryCDT.o: SharedMemoryCDT.c
	$(CC) $(CFLAGS) -c SharedMemoryCDT.c -o SharedMemoryCDT.o

slave.out: slave.c
	$(CC) $(CFLAGS) slave.c -o slave.out

app.o: app.c
	$(CC) $(CFLAGS) -c app.c -o app.o

appLib.o: appLib.c
	$(CC) $(CFLAGS) -c appLib.c -o appLib.o

app.out: app.o appLib.o SharedMemoryCDT.o
	$(CC) app.o appLib.o SharedMemoryCDT.o -o app.out

view.o: view.c
	$(CC) $(CFLAGS) -c view.c -o view.o

view.out: view.o SharedMemoryCDT.o
	$(CC) $(CFLAGS) view.o SharedMemoryCDT.o -o view.out

clean:
	rm -f *.o *.out results.txt
