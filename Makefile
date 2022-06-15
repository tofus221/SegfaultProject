CC = gcc -g -fsanitize=address

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= -Wall -Wextra -std=c99 -O3 
LDFLAGS=
LDLIBS= `pkg-config --libs sdl SDL_image` -lm

OBJ = pixelOp.o engine.o perlin.o agent.o main.o sickness.o food.o
DEP = ${OBJ:.o=.d}
EXE = main

all: main
main: ${OBJ}

clean:
	${RM} -r ${OBJ} ${DEP} ${EXE} .vscode

-include ${DEP}