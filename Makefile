INCLUDE=/usr/msp430/
CC=msp430-gcc
LD=msp430-ld
AR=msp430-ar
CFLAGS= -std=gnu99 -W -Os -g -mmcu=msp430f149 -pedantic -Wall -Wunused 

OBJ_FILES=\
	main.o utils.o

.PHONY: indent


TARGET=main
all: ${TARGET}.elf

# the math library will NOT be included unless -lm is the last argument
${TARGET}.elf : ${OBJ_FILES}	 Makefile
	${CC}  ${CFLAGS} -I${INCLUDE}  ${OBJ_FILES} \
	-o $@ -lm

%.s: %.c Makefile
	${CC} ${CFLAGS} -I ${INCLUDE} -S $<

install: ${TARGET}.elf
	mspdebug -j olimex "prog $<"
indent:
	indent -v *.[ch]
clean:
	-rm *.elf
	-rm *.o
	-rm *~
	-rm main.s

main.o: main.c

