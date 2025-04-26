# Compiler and flags
CC = arm-none-eabi-gcc
CFLAGS = -I./includes -O2 -Wall -mthumb -mcpu=cortex-m0plus ${DEVICE}
DEVICE = -DCPU_MKL46Z256VLL4

LDFLAGS = -Wall -mthumb -mcpu=cortex-m0plus --specs=nosys.specs ${DEVICE} -Wl,--gc-sections,-Map=$(TARGET).map,-T,link.ld

# Debug-specific flags
DEBUG_FLAGS = -O0 -g3

# Files
SRC = main.c startup.c lcd.c queue.c tasks.c timers.c port.c list.c heap_4.c stream_buffer.c fsl_tickless_systick.c system_MKL46Z4.c
OBJ = $(SRC:.c=.o)

# Output binary
TARGET = main

# Default target
all: $(TARGET).elf

# Linking
$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

# Compilation rule
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET).elf

# GDB target
gdb: $(TARGET).elf
	gdb-multiarch -ex "target extended-remote localhost:3333" $(TARGET).elf

# Flash target
flash: $(TARGET).elf
	openocd -f openocd.cfg -c "program $(TARGET).elf verify reset exit"

# Cleaning object files
clean:
	rm -rf $(OBJ)

# Cleaning everything
mrproper: clean
	rm -rf $(TARGET).elf $(TARGET).map