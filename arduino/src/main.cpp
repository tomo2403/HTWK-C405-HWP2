#include <Arduino.h>

uint8_t lastReceived = -1;

uint8_t reverseNibble(uint8_t nibble)
{
	nibble = (nibble & 0b0001) << 3 | // Bit 0 -> Bit 3
	         (nibble & 0b0010) << 1 | // Bit 1 -> Bit 2
	         (nibble & 0b0100) >> 1 | // Bit 2 -> Bit 1
	         (nibble & 0b1000) >> 3; // Bit 3 -> Bit 0
	return nibble;
}

uint8_t reverseByte(uint8_t byte)
{
	byte = (byte & 0b00000001) << 7 | // Bit 0 -> Bit 7
	       (byte & 0b00000010) << 5 | // Bit 1 -> Bit 6
	       (byte & 0b00000100) << 3 | // Bit 2 -> Bit 5
	       (byte & 0b00001000) << 1 | // Bit 3 -> Bit 4
	       (byte & 0b00010000) >> 1 | // Bit 4 -> Bit 3
	       (byte & 0b00100000) >> 3 | // Bit 5 -> Bit 2
	       (byte & 0b01000000) >> 5 | // Bit 6 -> Bit 1
	       (byte & 0b10000000) >> 7; // Bit 7 -> Bit 0
	return byte;
}

uint8_t readNibble()
{
	const uint8_t portD = (PIND & 0b11000000) >> 6; // Lese D6 und D7
	const uint8_t portB = PINB & 0b00000011; // Lese D8 und D9
	const uint8_t nibble = portB << 2 | portD;
	return reverseNibble(nibble);
}

void writeNibble(const uint8_t nibble)
{
	PORTD = reverseByte(PORTD & 0b11000011 | (nibble & 0b00001111) << 2);
}

void setup()
{
	Serial.begin(57600);
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, INPUT);
	pinMode(7, INPUT);
	pinMode(8, INPUT);
	pinMode(9, INPUT);
}

void loop()
{
	const uint8_t received = readNibble();
	const uint8_t received2 = readNibble();
	//_delay_us(1);
	if (received == received2 && received != lastReceived)
	{
		lastReceived = received;
		Serial.write(received);
	}

	if (Serial.available())
	{
		writeNibble(Serial.read());
	}
}
