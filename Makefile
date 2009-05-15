CC = gcc
EXTRA_CFLAGS ?=
EXTRA_LDFLAGS ?=
CFLAGS := $(shell pkg-config --cflags glib-2.0 gio-2.0 gtk+-2.0) -Wall -g -ansi -std=c99 $(EXTRA_CFLAGS)
LDFLAGS := $(shell pkg-config --libs glib-2.0 gio-2.0 gtk+-2.0 gthread-2.0 alsa) -lexpat $(EXTRA_LDFLAGS)
OBJECTS = gdigi.o gui.o effects.o preset.o gtkknob.o
DEPFILES = $(foreach m,$(OBJECTS:.o=),.$(m).m)

.PHONY : clean distclean all
%.o : %.c
	$(CC) $(CFLAGS) -c $<

.%.m : %.c
	$(CC) $(CFLAGS) -M -MF $@ -MG $<

all: gdigi

gdigi: $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $+

knob.h: knob.png
	gdk-pixbuf-csource --name=knob_pixbuf knob.png > $@

clean:
	rm -f *.o
	rm -f knob.h

distclean : clean
	rm -f .*.m
	rm -f gdigi

-include $(DEPFILES)
