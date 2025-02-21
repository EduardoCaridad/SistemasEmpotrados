# Variables
CC = arm-none-eabi-gcc
CFLAGS = -I./includes -O2 -Wall -mthumb -mcpu=cortex-m0plus
LDFLAGS = -O2 -Wall -Wextra -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map,main.map -Tlink.ld
TARGET = main.elf
OPENOCD_CFG = openocd.cfg

SRCS = startup.c practica1.c
OBJS = $(SRCS:.c=.o)

# Enlazar los objetos y generar el ELF
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# Subir a la placa
flash:
	openocd -f $(OPENOCD_CFG) -c "program $(TARGET) verify reset exit"

# Limpiar
clean:
	rm -f $(OBJS) $(TARGET) main.map
