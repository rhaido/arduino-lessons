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
CFLAGS=-Wall -Wno-main -Os -DF_CPU=$(F_CPU) -mmcu=$(MCU)
OBJECTS=main.c 4bit_lcd.c
SUDO=sudo

TARGET=main.ihex

%.out : main.c 4bit_lcd.c
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

%.ihex : %.out
	$(OBJCOPY) -O $(BIN_FORMAT) -R .eeprom $< $@

%.upload : %.ihex
	$(SUDO) $(AVRDUDE) -q -p$(MCU) -c$(PROTOCOL) -P$(PORT) -b$(BAUD) -Uflash:w:$<:i

clean:
	rm *.out *.ihex
