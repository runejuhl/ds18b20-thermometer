FERRET  = ferret
INO     = ds18b20-thermometer.ino
ARDUINO = ~/bin/arduino
BOARD   = arduino:avr:uno
PORT    = /dev/ttyACM0
RM      = rm -f

.PHONY: verify upload clean

default: verify

verify: $(INO)
	$(ARDUINO) --board $(BOARD) --verify $(INO)

upload: $(INO)
	$(ARDUINO) --board $(BOARD) --port $(PORT) --upload $(INO)

clean:
	$(RM) $(INO)

serial:
	picocom -b 9600 --imap lfcrlf $(PORT)
