CC = gcc -g

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= -Wall -Wextra -std=c99 -O3 
LDFLAGS=
LDLIBS= `pkg-config --libs sdl SDL_image` -lm

OBJ = pixelOp.o engine.o main.o agent.o
DEP = ${OBJ:.o=.d}
EXE = main

all: main
main: ${OBJ}

clean:
	${RM} ${OBJ} ${DEP} ${EXE}

-include ${DEP}