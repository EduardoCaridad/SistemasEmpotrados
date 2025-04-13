CFLAGS = -I./includes -I./drivers -O2 -Wall -Wformat=2 -mthumb -mcpu=cortex-m0plus
DFLAGS = -I./includes -I./drivers -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus
LDFLAGS = -Wl,--gc-sections,-Map=main.map,-T,link.ld --specs=nano.specs

SRCS = main.c startup.c drivers/lcd.c
OBJS = main.o startup.o drivers/lcd.o

# Producción
main.elf: $(OBJS)
	arm-none-eabi-gcc $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

# Depuración
main_debug.elf: $(SRCS)
	arm-none-eabi-gcc $(DFLAGS) -c -o main.o main.c
	arm-none-eabi-gcc $(DFLAGS) -c -o startup.o startup.c
	arm-none-eabi-gcc $(DFLAGS) -c -o drivers/lcd.o drivers/lcd.c
	arm-none-eabi-gcc $(DFLAGS) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c -o $@ $<

gdb: main_debug.elf
	gdb-multiarch -ex "target extended-remote localhost:3333" main_debug.elf


flash_debug: main_debug.elf
	openocd -f openocd.cfg -c "program main_debug.elf verify reset exit"

flash: main.elf
	openocd -f openocd.cfg -c "program main.elf verify reset exit"

clean:
	rm -rf $(OBJS)

mrproper:
	make clean
	rm -rf main.elf main_debug.elf main.map

cppcheck:
	cppcheck --enable=all --inconclusive --std=c99 --force --inline-suppr \
	-I./includes -I./drivers main.c



