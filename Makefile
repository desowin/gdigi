CC = gcc
CFLAGS = `pkg-config --cflags glib-2.0 gio-2.0 gtk+-2.0` -Wall -g -ansi
OFLAG = -o
LIBS = `pkg-config --libs glib-2.0 gio-2.0 gtk+-2.0 alsa` -lexpat

.SUFFIXES : .o .c
.c.o :
	$(CC) $(CFLAGS) -c $<

all: gdigi

gdigi: gdigi.o tests.o gui.o effects.o preset.o gtkknob.o
	$(CC) $(LIBS) $(OFLAG) gdigi gdigi.o tests.o gui.o effects.o preset.o gtkknob.o

gdigi.o: gdigi.c

tests.o: tests.c

gui.o: gui.c

effects.o: effects.c

preset.o: preset.c

gtkknob.o: gtkknob.c

clean:
	rm *.o


