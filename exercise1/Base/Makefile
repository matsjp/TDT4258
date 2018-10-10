# Example Makefile
#
# Exercise 1, TDT4258

LD=arm-none-eabi-gcc
AS=arm-none-eabi-as
OBJCOPY=arm-none-eabi-objcopy

LDFLAGS=-nostdlib
ASFLAGS=-mcpu=cortex-m3 -mthumb -g

LINKERSCRIPT=efm32gg.ld

.PHONY : upload
upload : ex1.bin
	-eACommander.sh -r --address 0x00000000 -f $^ -r

ex1.bin : ex1.elf
	${OBJCOPY} -j .text -O binary $< $@

ex1.elf : ex1.o 
	${LD} -T ${LINKERSCRIPT} $^ -o $@ ${LDFLAGS} 

ex1.o : ex1.s
	${AS} ${ASFLAGS} $< -g -o $@
	
.PHONY : clean
clean :
	-rm -rf *.o *.elf *.bin *.hex *~
