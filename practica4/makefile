CC = arm-none-eabi-gcc
CFLAGS = -I./includes -I./drivers -O2 -Wall -mthumb -mcpu=cortex-m0plus -DCPU_MKL46Z128VLH4
LDFLAGS = -O2 -Wall -Wextra -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map=output.map -Tlink.ld

SRC_COMMON = main.c startup.c drivers/lcd.c

# reverse_int (ORIXINAL)
SRC_OPT = $(SRC_COMMON) reverse_int_opt.s
OBJ_OPT = build/main.o build/startup.o build/drivers/lcd.o build/reverse_int_opt.o

# reverse_int_opt (OPTIMIZADA)
SRC_REV = $(SRC_COMMON) reverse_int.s
OBJ_REV = build/main.o build/startup.o build/drivers/lcd.o build/reverse_int.o

TARGET = practica4.elf

OPENOCD_CFG = openocd.cfg

all: $(REVERSE_INT_MAIN) practica4_build

REVERSE_INT_MAIN = p4.out

# COMPILAR O ARQUIVO .c
run_reverse_int_main:
	gcc -o $(REVERSE_INT_MAIN) reverse_int_main.c
	@echo "Ejecutando reverse_int_main para mostrar resultados..."
	./$(REVERSE_INT_MAIN)
	@echo "\nTodo perfecto, aquí comprobase como debería resultar o número ao flashear na placa e realizar a operación."
	@sleep 10

build/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

build/drivers/%.o: drivers/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# COMPILAR reverse_int_opt.s
build/reverse_int_opt.o: reverse_int_opt.s
	$(CC) $(CFLAGS) -c -o $@ $<

# COMPILAR reverse_int.s
build/reverse_int.o: reverse_int.s
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET)_opt: $(OBJ_OPT)
	$(CC) $(LDFLAGS) $(OBJ_OPT) -o $(TARGET)

$(TARGET)_rev: $(OBJ_REV)
	$(CC) $(LDFLAGS) $(OBJ_REV) -o $(TARGET)

flash_opt: run_reverse_int_main $(TARGET)_opt
	openocd -f $(OPENOCD_CFG) -c "program $(TARGET) verify reset exit"

flash_rev: run_reverse_int_main $(TARGET)_rev
	openocd -f $(OPENOCD_CFG) -c "program $(TARGET) verify reset exit"

# CLEAN
clean:
	rm -f build/*.o build/drivers/*.o $(REVERSE_INT_MAIN) $(TARGET) *.map
	rm -rf build/*.o build/drivers/*.o