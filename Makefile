#avr-gcc -Os -mmcu=atmega328p -o flash_diod.out flash_diod.c
#avr-objcopy -O ihex -R .eeprom flash_diod.out flash_diod.hex
#avrdude -q -patmega328p -carduino -P/dev/ttyACM0 -b115200 -Uflash:w:flash_diod.hex:i

AVRDUDE=avrdude -F -V
OBJCOPY=avr-objcopy
CC=avr-gcc
RM=rm -f

# parameters
MCU=atmega328p
F_CPU=16000000UL
BIN_FORMAT=ihex
PORT=/dev/ttyACM0
BAUD=115200
PROTOCOL=arduino
CFLAGS=-Wall -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU) -Wl,-u,vfprintf -lprintf_min
SUDO=sudo

TARGET=main.ihex

%.out : %.c
	$(CC) $(CFLAGS) -o $@ $< 

%.ihex : %.out
	$(OBJCOPY) -O $(BIN_FORMAT) -R .eeprom $< $@

%.upload : %.ihex
	$(SUDO) $(AVRDUDE) -q -p$(MCU) -c$(PROTOCOL) -P$(PORT) -b$(BAUD) -Uflash:w:$<:i

clean:
	rm *.out *.ihex
