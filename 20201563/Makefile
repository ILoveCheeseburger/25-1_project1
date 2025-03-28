CC = gcc
CFLAGS = -g -Wall -std=c99
OBJS = main.o list.o hash.o bitmap.o
TARGET = sp_prj1

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c list.h hash.h bitmap.h
list.o: list.c list.h
hash.o: hash.c hash.h
bitmap.o: bitmap.c bitmap.h

clean:
	rm -f *.o $(TARGET)
