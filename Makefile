CC=gcc
CFLAGS=-std=c99 -Wall -Wextra
LOAD=load_balancer
SERVER=server
LISTS=lists
HASHT=hashtable

ZIPNAME := load_balancer.zip

SOURCES=main.c $(LOAD).c $(SERVER).c $(LISTS).c $(HASHT).c
HEADERS=$(LOAD).h $(SERVER).h $(LISTS).h $(HASHT).h utils.h

.PHONY: build clean pack

build: main.o $(LOAD).o $(SERVER).o $(LISTS).o $(HASHT).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(LISTS).o: $(LISTS).c $(LISTS).h
	$(CC) $(CFLAGS) $^ -c

$(HASHT).o: $(HASHT).c $(HASHT).h
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f *.o tema2 *.h.gch

pack:
	zip -FSr $(ZIPNAME) $(SOURCES) $(HEADERS) Makefile README README.md
