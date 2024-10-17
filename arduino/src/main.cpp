#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    pinMode(4, INPUT);
    pinMode(5, INPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);

	/// TODO: Ard zu B15 schreiben
}

void loop() {
    delay(1000);

    uint8_t input = 0x0;

    for (int i = 0; i < 4; i++) {
        // Lese den Zustand des Pins (2+i)
        uint8_t pinValue = digitalRead(2 + i);
        // Setze das entsprechende Bit in input, wenn der Pin HIGH ist
        if (pinValue == HIGH) {
            input |= (0x1 << i);
        }
    }

    Serial.println(input);
}
