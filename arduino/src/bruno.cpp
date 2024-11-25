// B15 >---(2,3,4,5)---> Arduino
// B15 <---(6,7,8,9)---< Arduino

#include "Arduino.h"

int previousNibble = 0;  // 4-bit previous nibble
int currentNibble = 0;   // 4-bit current nibble

void setup() {
	Serial.begin(57600);  // 115200 9600 Start serial communication with the PC
	// Set D2-D5 as input pins (for receiving data)
	for (int i = 2; i <= 5; i++) {
		pinMode(i, INPUT);
	}
	// Set D6-D9 as output pins (for sending data)
	for (int i = 6; i <= 9; i++) {
		pinMode(i, OUTPUT);
	}
	for (int i = 2; i <= 5; i++) {
	  currentNibble |= (digitalRead(i) << (i - 2));  // Shift bits into place
	}
	previousNibble = currentNibble;
}

void loop() {
	// Read the current nibble from D2-D5
	currentNibble = 0;
	for (int i = 2; i <= 5; i++) {
		currentNibble |= (digitalRead(i) << (i - 2));  // Shift bits into place
	}

	// If the nibble has changed, send it to the PC
	if (currentNibble != previousNibble) {

		//delay(10);

		currentNibble = 0;
		for (int i = 2; i <= 5; i++) {
			currentNibble |= (digitalRead(i) << (i - 2));  // Shift bits into place
		}

		Serial.println(currentNibble, BIN);
		//Serial.write(currentNibble);  // Send the current nibble as a byte
		previousNibble = currentNibble;
	}

	// Check if there is new data from PC2 via the serial port
	if (Serial.available() > 0) {
		// Read the data byte from the serial port (received data from PC2)
		byte dataToSend = Serial.read();

		// Send the byte back to PC1 through D6-D9 (sending 4 bits at a time)
		for (int i = 6; i <= 9; i++) {
			// Send the bits of the byte to the output pins D6-D9
			digitalWrite(i, (dataToSend >> (i - 6)) & 1);  // Extract and set each bit
		}
	}
}
