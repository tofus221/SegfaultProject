CC = gcc

CPPFLAGS= `pkg-config --cflags sdl` -MMD
CFLAGS= `pkg-config --cflags gtk+-3.0` -Wall -Wextra -std=c99 -O3 
LDFLAGS=
LDLIBS= `pkg-config --libs sdl gtk+-3.0 SDL_image` -lm -lgtk-3

OBJ = pixelOp.o engine.o perlin.o agent.o main.o sickness.o food.o interface.o stats.o
DEP = ${OBJ:.o=.d}
EXE = main

all: main
main: ${OBJ}

clean:
	${RM} -r ${OBJ} ${DEP} ${EXE} .vscode

-include ${DEP}