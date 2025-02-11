
.PHONY: compile run

compile:
	arm-none-eabi-gcc -I ./includes -O2 -Wall -mthumb -mcpu=cortex-m0plus -c -o startup.o startup.c
	arm-none-eabi-gcc -I ./includes -O2 -Wall -mthumb -mcpu=cortex-m0plus -c -o main.o main.c
	arm-none-eabi-gcc -I ./includes -O2 -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map,main.map,-Tlink.ld main.o startup.o -o main.elf


compile-dbg:
	arm-none-eabi-gcc -I ./includes -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus -c -o startup.o startup.c
	arm-none-eabi-gcc -I ./includes -O0 -g3 -Wall -mthumb -mcpu=cortex-m0plus -c -o main.o main.c
	arm-none-eabi-gcc -I ./includes -O0 -g -Wall -mthumb -mcpu=cortex-m0plus --specs=nano.specs -Wl,--gc-sections,-Map,main.map,-Tlink.ld main.o startup.o -o main.elf

run:
	openocd -f openocd.cfg -c "program main.elf verify reset exit"

debug:
	gdb-multiarch -ex "target extended-remote localhost:3333" main.elf
