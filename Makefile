CC=gcc
FLAGS=--std=c99 -O2 -ggdb -pthread
ARTIFACT=nts

.DEFAULT_GOAL := build

.PHONY: build clean

build: ${ARTIFACT}

${ARTIFACT}:
	  ${CC} ${FLAGS} ./*.c -o ${ARTIFACT}

clean:
	  rm ./${ARTIFACT}
