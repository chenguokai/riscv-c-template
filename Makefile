####################################
# This intends to be a simple test suit framework for RISC-V bare metal/Linux environment
# licensed under BSD-3-Clause
####################################

# Define the source files
SOURCES = main.c start.S

# Define the output files
OUTPUT_ELF_CROSS = main.elf
OUTPUT_DISASM_CROSS = main.disasm
OUTPUT_BINARY_CROSS = main.bin
OUTPUT_ELF_NATIVE = main_native.elf
OUTPUT_DISASM_NATIVE = main_native.disasm
OUTPUT_BINARY_NATIVE = main_native.bin

# Define the cross toolchain and flags
CROSS_PATH ?= /nfs/home/share/riscv/bin/
PREFIX ?= riscv64-unknown-elf-

CROSS_CC = ${CROSS_PATH}/${PREFIX}gcc
CROSS_LD = ${CROSS_PATH}/${PREFIX}ld
CROSS_OBJDUMP = ${CROSS_PATH}/${PREFIX}objdump
CROSS_OBJCOPY = ${CROSS_PATH}/${PREFIX}objcopy
CROSS_CFLAGS = -g -O0 -nostdlib -mcmodel=medany
CROSS_LDFLAGS = -T linker.ld

# Define the native toolchain and flags
NATIVE_CC = gcc
NATIVE_LD = ld
NATIVE_OBJDUMP = objdump
NATIVE_OBJCOPY = objcopy
NATIVE_CFLAGS = -Wall -Werror -g -O0 -DNATIVE
NATIVE_LDFLAGS =

# Define the default target
all: $(OUTPUT_ELF_CROSS) $(OUTPUT_DISASM_CROSS) $(OUTPUT_BINARY_CROSS) \
     $(OUTPUT_ELF_NATIVE) $(OUTPUT_DISASM_NATIVE) $(OUTPUT_BINARY_NATIVE)

native: $(OUTPUT_ELF_NATIVE) $(OUTPUT_DISASM_NATIVE) $(OUTPUT_BINARY_NATIVE)

cross: $(OUTPUT_ELF_CROSS) $(OUTPUT_DISASM_CROSS) $(OUTPUT_BINARY_CROSS)

# Define the cross toolchain targets
$(OUTPUT_ELF_CROSS): $(SOURCES)
	$(CROSS_CC) $(CROSS_CFLAGS) -c $^
	$(CROSS_LD) $(CROSS_LDFLAGS) *.o -o $@

$(OUTPUT_DISASM_CROSS): $(OUTPUT_ELF_CROSS)
	$(CROSS_OBJDUMP) -d $^ > $@

$(OUTPUT_BINARY_CROSS): $(OUTPUT_ELF_CROSS)
	$(CROSS_OBJCOPY) -O binary $^ $@

# Define the native toolchain targets
$(OUTPUT_ELF_NATIVE): $(SOURCES)
	$(NATIVE_CC) $(NATIVE_CFLAGS) $^ -o $@

$(OUTPUT_DISASM_NATIVE): $(OUTPUT_ELF_NATIVE)
	$(NATIVE_OBJDUMP) -d $^ > $@

$(OUTPUT_BINARY_NATIVE): $(OUTPUT_ELF_NATIVE)
	$(NATIVE_OBJCOPY) -O binary $^ $@

clean:
	rm *.elf
	rm *.bin
	rm *.disass
	rm *.o
