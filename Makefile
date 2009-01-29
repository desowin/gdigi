CC = gcc
CFLAGS = `pkg-config --cflags glib-2.0 gtk+-2.0` -Wall -g
OFLAG = -o
LIBS = `pkg-config --libs glib-2.0 gtk+-2.0 libusb`

.SUFFIXES : .o .c
.c.o :
	$(CC) $(CFLAGS) -c $<

all: gdigi

gdigi: gdigi.o
	$(CC) $(LIBS) $(OFLAG) gdigi gdigi.o

gdigi.o: gdigi.c

clean:
	rm *.o


