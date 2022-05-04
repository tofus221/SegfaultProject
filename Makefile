CC = gcc -g

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= `pkg-config --cflags gtk+-3.0` -Wall -Wextra -std=c99 -O3 
LDFLAGS=
LDLIBS= `pkg-config --libs sdl gtk+-3.0` -lm -lgtk-3


OBJ = pixelOp.o engine.o perlin.o main.o interface.o
DEP = ${OBJ:.o=.d}
EXE = main

all: main
main: ${OBJ}

clean:
	${RM} ${OBJ} ${DEP} ${EXE}

-include ${DEP}