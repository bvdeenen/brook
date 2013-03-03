INCLUDE=/usr/msp430/
CC=msp430-gcc
LD=msp430-ld
AR=msp430-ar
CFLAGS= -std=gnu99 -W -Os -g -mmcu=msp430f149 -pedantic -Wall -Wunused

OBJ_FILES=\
	main.o

TARGET=main
all: ${TARGET}.elf

${TARGET}.elf : ${OBJ_FILES}	 Makefile
	${CC} ${CFLAGS} -I${INCLUDE}  ${OBJ_FILES} \
	-o $@

install: ${TARGET}.elf
	mspdebug -j olimex "prog $<"

clean:
	-rm *.elf
	-rm *.o

main.o: main.c

