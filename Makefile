CC = gcc
CFLAGS = `pkg-config --cflags glib-2.0 gio-2.0 gtk+-2.0` -Wall -g -ansi -std=c99
OFLAG = -o
LIBS = `pkg-config --libs glib-2.0 gio-2.0 gtk+-2.0 alsa` -lexpat

.SUFFIXES : .o .c
.c.o :
	$(CC) $(CFLAGS) -c $<

all: gdigi

gdigi: knob.h gdigi.o gui.o effects.o preset.o gtkknob.o
	$(CC) $(LIBS) $(OFLAG) gdigi gdigi.o gui.o effects.o preset.o gtkknob.o

gdigi.o: gdigi.c

gui.o: gui.c

effects.o: effects.c

preset.o: preset.c

gtkknob.o: gtkknob.c

gtkknob.o: gtkknob.c

knob.h:
	gdk-pixbuf-csource --name=knob_pixbuf knob.png > $@

clean:
	rm *.o


