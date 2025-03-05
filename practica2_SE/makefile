# Variables
CC = arm-none-eabi-gcc

# -DCPU MKL46Z128VLH4 --> Identificador de placa para fsl_device_registers.h
CFLAGS = -I./includes -I./drivers -O2 -Wall -mthumb -mcpu=cortex-m0plus -DCPU_MKL46Z128VLH4


LDFLAGS = -O2 -Wall -Wextra -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=output.map -Tlink.ld


# Drivers 
SRC = $(wildcard drivers/*.c)
OBJ = $(patsubst drivers/%.c, drivers/%.o, $(SRC))

# Hello World
OBJS_HELLO = startup.o hello_world/hello_world.o hello_world/board.o hello_world/clock_config.o hello_world/pin_mux.o $(OBJ)

# Led Blinky
OBJS_BLINKY = startup.o led_blinky/led_blinky.o led_blinky/board.o led_blinky/clock_config.o led_blinky/pin_mux.o $(OBJ)

# Targets
TARGET_HELLO = hello_world.elf
TARGET_BLINKY = led_blinky.elf

#Flasheo
OPENOCD_CFG = openocd.cfg

# Compilar y enlazar
all: $(TARGET_HELLO) $(TARGET_BLINKY)

# Regla para compilar archivos fuente a objetos
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# Link
$(TARGET_HELLO): $(OBJS_HELLO)
	$(CC) $(LDFLAGS) $(OBJS_HELLO) -o $(TARGET_HELLO)

$(TARGET_BLINKY): $(OBJS_BLINKY)
	$(CC) $(LDFLAGS) $(OBJS_BLINKY) -o $(TARGET_BLINKY)


# Flash a la placa:

flash_hello: $(TARGET_HELLO)
	openocd -f $(OPENOCD_CFG) -c "program $(TARGET_HELLO) verify reset exit"

flash_led: $(TARGET_BLINKY)
	openocd -f $(OPENOCD_CFG) -c "program $(TARGET_BLINKY) verify reset exit"

# Clean
clean:
	rm -f $(OBJS_HELLO) $(OBJS_BLINKY) $(TARGET_HELLO) $(TARGET_BLINKY) *.map startup.o drivers/*.o
