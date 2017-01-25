CC=gcc
FLAGS=--std=c99 -O2 -ggdb
ARTIFACT=nes

.DEFAULT_GOAL := build

.PHONY: build clean

build: ${ARTIFACT}

${ARTIFACT}: main.c rom.c rom.h
	  ${CC} ${FLAGS} main.c rom.c -o ${ARTIFACT}

clean:
	  rm ./${ARTIFACT}
	  rm ./*.gch
