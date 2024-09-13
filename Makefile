CC = gcc
CFLAGS = -Wall
TARGETS = slave.out app.out

all: $(TARGETS)

slave.out: slave.c
	$(CC) $(CFLAGS) slave.c -o slave.out

app.o: app.c
	$(CC) $(CFLAGS) -c app.c -o app.o

appLib.o: appLib.c
	$(CC) $(CFLAGS) -c appLib.c -o appLib.o

app.out: app.o appLib.o
	$(CC) app.o appLib.o -o app.out

clean:
	rm -f *.o *.out
