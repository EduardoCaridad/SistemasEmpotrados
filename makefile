# Variables
CC = arm-none-eabi-gcc

# Flags de compilación
CFLAGS = -I./includes -I./drivers -O2 -Wall -mthumb -mcpu=cortex-m0plus -DCPU_MKL46Z128VLH4

# Flags de enlace
LDFLAGS = -O2 -Wall -Wextra -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=output.map -Tlink.ld

# Drivers y archivos fuente
SRC = $(wildcard drivers/*.c) main.c startup.c
OBJ = $(SRC:.c=.o)

# Nombre del archivo final
TARGET = practica3.elf

# Archivo de configuración de OpenOCD
OPENOCD_CFG = openocd.cfg

# Compilar y enlazar
all: $(TARGET)

# Regla para compilar archivos fuente a objetos
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Link
$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(TARGET)

# Flash a la placa
flash: $(TARGET)
	openocd -f $(OPENOCD_CFG) -c "program $(TARGET) verify reset exit"

# Clean
clean:
	rm -f $(OBJ) $(TARGET) *.map