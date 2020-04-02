
BUILD_DIR=./build/
AVR_CONF=../avrdude.conf
USB_MODEM := $(shell ls /dev/tty.usb*)

build:
	mkdir build
	avra main.asm
	mv *.hex $(BUILD_DIR)
	mv *.obj $(BUILD_DIR)

run:
	avrdude -C $(AVR_CONF) -v -p atmega328p -c arduino -P $(USB_MODEM) -b 115200 -D -U flash:w:$(BUILD_DIR)main.hex:i

clean:
	rm -rf build/
	rm *.obj *.hex
