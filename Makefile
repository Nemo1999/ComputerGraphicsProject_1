#CPPFLAGS=$(shell sdl2-config --cflags) $(EXTRA_CPPFLAGS)
#LDLIBS=$(shell sdl2-config --libs) -lGLEW $(EXTRA_LDLIBS)
#CPPFLAGS=-L /lib/x86_64-linux-gnu
CC=gcc
LDLIBS=-lGLEW -lglfw -lGL -lm

#EXTRA_LDLIBS?=-lGL
all: Guide
	./Guide

Guide: shader.vert shader.frag Guide.c LinAlg.h
	gcc Guide.c ${LDLIBS} -o Guide

clean:
	rm -f *.o *~   Guide
.PHONY: all clean
