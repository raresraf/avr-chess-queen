all: sah.hex

sah.hex: sah.elf
	avr-objcopy  -j .text -j .data -O ihex $^ $@
	avr-size $^

sah.elf: sah.c usart.c
	avr-g++ -mmcu=atmega324p -DF_CPU=16000000 -Os -Wall -o $@ $^

clean:
	rm -rf *.elf *.hex
