#include "../../lib/crc.hpp"

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

	// TODO: Ard zu B15 schreiben
}

// nur die letzten 4 bits werden beachtet
void digitalWriteAll(const uint8_t value) {
    digitalWrite(6, (value & 0x01));
    digitalWrite(7, (value >> 1 & 0x01));
    digitalWrite(8, (value >> 2 & 0x01));
    digitalWrite(9, (value >> 3 & 0x01));
}

void loop() {
    uint8_t input = 0x0;

    for (int i = 0; i < 4; i++) {
        // Lese den Zustand des Pins (2+i)
        uint8_t pinValue = digitalRead(2 + i);
        // Setze das entsprechende Bit in input, wenn der Pin HIGH ist
        if (pinValue == HIGH) {
            input |= (0x1 << i);
        }
    }

	digitalWriteAll(0x07);
    
    Serial.println(input);
}