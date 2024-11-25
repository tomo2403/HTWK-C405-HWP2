#include <Arduino.h>

uint8_t lastReceived = 0;

uint8_t readNibble() {
	// D2 to D5
    return (PIND & 0b00111100) >> 2;
}

void writeNibble(uint8_t nibble){
	// D6 to D9
	PORTD = (PORTD & 0b11000011) | ((nibble & 0b00001111) << 2);
}

void setup()
{
	Serial.begin(57600);
}

void loop()
{
	uint8_t received = readNibble();
	if(received != lastReceived){
		Serial.write(received);
		lastReceived = received;
	}

	if (Serial.available()){
		writeNibble(Serial.read());
	}
}