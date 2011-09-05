CC = gcc
EXTRA_CFLAGS ?=
EXTRA_LDFLAGS ?=
CFLAGS := $(shell pkg-config --cflags glib-2.0 gio-2.0 gtk+-2.0) -Wall -g -ansi -std=c99 $(EXTRA_CFLAGS)
LDFLAGS = $(EXTRA_LDFLAGS) -Wl,--as-needed
LDADD := $(shell pkg-config --libs glib-2.0 gio-2.0 gtk+-2.0 gthread-2.0 alsa) -lexpat 
OBJECTS = gdigi.o gui.o effects.o preset.o gtkknob.o
DEPFILES = $(foreach m,$(OBJECTS:.o=),.$(m).m)

.PHONY : clean distclean all
%.o : %.c
	$(CC) $(CFLAGS) -c $<

.%.m : %.c
	$(CC) $(CFLAGS) -M -MF $@ -MG $<

all: gdigi

gdigi: $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $+ $(LDADD)

knob.h: knob.png
	gdk-pixbuf-csource --name=knob_pixbuf knob.png > $@

clean:
	rm -f *.o
	rm -f knob.h

distclean : clean
	rm -f .*.m
	rm -f gdigi

install: gdigi
	install gdigi $(DESTDIR)/usr/bin
	install gdigi.desktop $(DESTDIR)/usr/share/applications/

NODEP_TARGETS := clean distclean
depinc := 1
ifneq (,$(filter $(NODEP_TARGETS),$(MAKECMDGOALS)))
depinc := 0
endif
ifneq (,$(fitler-out $(NODEP_TARGETS),$(MAKECMDGOALS)))
depinc := 1
endif

ifeq ($(depinc),1)
-include $(DEPFILES)
endif
