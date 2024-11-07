#include "../../lib/crc.hpp"

uint8_t lastPinReceived = 0;
uint8_t usbReceived = 0;

void setup() {
    Serial.begin(57600);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
}

// nur die letzten 4 bits werden beachtet
void digitalWriteAll(const uint8_t value) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(6 + i, (value >> i) & 0x01);
    }
}

uint8_t digitalReadAll() {
    uint8_t value = 0;
    for (int i = 0; i < 4; i++) {
        if (digitalRead(2 + i) == HIGH) {
            value |= (1 << i);
        }
    }
    return value;
}

void serialWriteAll(const uint8_t value) {
	Serial.write(value);
}

uint8_t serialReadAll() {
	if (Serial.available() > 0) {
		return Serial.read();
	}
	return 0;
}

void loop() {
	uint8_t pinReceived = digitalReadAll();
	if (pinReceived != lastPinReceived) {
		lastPinReceived = pinReceived;
		serialWriteAll(pinReceived);
	}

	while (Serial.available() > 0) {
		usbReceived = Serial.read();
		digitalWriteAll(usbReceived >> 4);
		digitalWriteAll(usbReceived);
	}
}