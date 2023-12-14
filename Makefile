default:
	avr-gcc -Os -Wall -DF_CPU=16000000UL -mmcu=atmega2560 -c -o steppers.o steppers.c
	avr-gcc -Os -Wall -DF_CPU=16000000UL -mmcu=atmega2560 -c -o segments.o segments.c
	avr-gcc -Os -Wall -DF_CPU=16000000UL -mmcu=atmega2560 -c -o main.o main.c
	avr-gcc -Os -Wall -DF_CPU=16000000UL -mmcu=atmega2560 main.o steppers.o segments.o
	avr-objcopy -O ihex -R .eeprom -j .text -j .data a.out a.hex
	sudo avrdude -F -V -c avrisp2 -p m2560 -P /dev/ttyACM0 -b 115200 -D -U flash:w:a.hex
