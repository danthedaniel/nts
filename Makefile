CC=gcc
FLAGS=--std=c99 -O2 -ggdb
ARTIFACT=nes

.DEFAULT_GOAL := build

.PHONY: build clean

build: ${ARTIFACT}

${ARTIFACT}: main.c rom.c rom.h cpu.h cpu.c apu.h apu.c ppu.h ppu.c console.h util.h util.c
	  ${CC} ${FLAGS} main.c rom.c cpu.c apu.c ppu.c util.c -o ${ARTIFACT}

clean:
	  rm ./${ARTIFACT}
