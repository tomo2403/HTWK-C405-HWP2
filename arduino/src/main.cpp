#include <pt.h>
#include "../../lib/Packets.hpp"

#define CHANNEL1 2
#define CHANNEL2 6

static struct pt thread1, thread2;

uint8_t lastChValueRec[CHANNEL2];

uint8_t digitalReadAll(uint8_t channel) {
	uint8_t value = 0;
	for (int i = 0; i < 4; i++) {
		if (digitalRead(channel + i) == HIGH) {
			value |= (1 << i);
		}
	}
	return value;
}

void digitalWriteAll(const uint8_t value) {
	for (int i = 0; i < 4; i++) {
		digitalWrite(6 + i, (value >> i) & 0x01);
	}
}

void changeChannelDirection(uint8_t channel, uint8_t direction) {
	for (int i = 0; i < 4; i++) {
		pinMode(channel + i, direction);
	}
}

StreamPacket receiveStreamPacket() {
	StreamPacket packet;

	// Header lesen
	while (Serial.available() < 3); // Warten, bis genügend Bytes verfügbar sind
	packet.channel = Serial.read();
	packet.type = static_cast<PacketType>(Serial.read());
	packet.dataLength = Serial.read();

	// Daten lesen
	while (Serial.available() < packet.dataLength); // Warten, bis alle Daten verfügbar sind
	Serial.readBytes(packet.data.data(), packet.dataLength);

	return packet;
}

int processChannel(uint8_t channel, struct pt *pt) {
	PT_BEGIN(pt);

	uint8_t pinReceived = digitalReadAll(channel);
	if (pinReceived != lastChValueRec[channel]) {
		lastChValueRec[channel] = pinReceived;

		Serial.write(channel);
		Serial.write(pinReceived);
	}

	if (Serial.available() > 4){
		StreamPacket sp = receiveStreamPacket();
		changeChannelDirection(channel, OUTPUT);
		for (uint8_t byte : sp.data) {
			digitalWriteAll(byte);
		}
		changeChannelDirection(channel, INPUT);
	}
	PT_END(pt);
}

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

	PT_INIT(&thread1);
	PT_INIT(&thread2);
}

void loop() {
	processChannel(CHANNEL1, &thread1);
	processChannel(CHANNEL2, &thread2);
}