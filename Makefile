CC = gcc
CFLAGS  = -g3 -Wall
LIBS += -lm


bitcoin:  main.o hash.o bitcoin.o transaction.o
	$(CC) $(CFLAGS) -o bitcoin main.o hash.o bitcoin.o transaction.o $(LIBS)

wallet.o:  hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c

bitcoin.o:  bitcoin.c bitcoin.h
	$(CC) $(CFLAGS) -c bitcoin.c

transaction.o:  transaction.c transaction.h
	$(CC) $(CFLAGS) -c transaction.c	

clean: 
	-rm -f *.o 
	-rm -f bitcoin
