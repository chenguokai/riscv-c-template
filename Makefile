####################################
# This intends to be a simple test suit for RISC-V Vector Extension (RVV)
# licensed under BSD-3-Clause
####################################

####################
# basic info
####################

CROSS_PATH = /home/xim/ACS/clang_rvv/bin
PREFIX = llvm-
CC = ${CROSS_PATH}/clang-12
OBJCOPY = ${CROSS_PATH}/${PREFIX}objcopy
OBJDUMP = ${CROSS_PATH}/${PREFIX}objdump
LDSCRIPT = linker.ld
C_SOURCES = $(shell find ./ -name '*.c')

# BUILD_DIR = build

# Note that we use clang to compile RVV assembly
CFLAGS = -menable-experimental-extensions -march=rv64gv1p0   -nostdlib -T ${LDSCRIPT} -ffreestanding  -mabi=lp64  -mcmodel=medium 

####################
# build rules
####################

OBJECTS = $(notdir $(C_SOURCES:.c=.o))
OBJECTS_BIN = $(OBJECTS:.elf=.bin)
OBJECTS_DISASSEMBLY = $(OBJECTS:.elf=.disass)

all: VIRTUAL_TARGET

${OBJECTS}: %.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

final.elf: ${OBJECTS}
	${CC} ${OBJECTS} ${CFLAGS} -o final.elf

%.bin: final.elf
	$(OBJCOPY) -O binary $< $@

%.disass: final.elf
	$(OBJDUMP) -d $< > $@

DEBUG: ${OBJECTS}
	@echo "DEBUG"
	@echo $<

VIRTUAL_TARGET: ${DEBUG} final.elf ${OBJECTS_BIN} ${OBJECTS_DISASSEMBLY}
	@echo "Voila!"

clean:
	rm *.elf
	rm *.bin
	rm *.disass
	rm *.o
