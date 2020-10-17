#CPPFLAGS=$(shell sdl2-config --cflags) $(EXTRA_CPPFLAGS)
#LDLIBS=$(shell sdl2-config --libs) -lGLEW $(EXTRA_LDLIBS)
#CPPFLAGS=-L /lib/x86_64-linux-gnu

LDLIBS=-lGLEW -lglfw -lGL -lm

#EXTRA_LDLIBS?=-lGL
all: Guide
clean:
	rm -f *.o *~   Guide
.PHONY: all clean
