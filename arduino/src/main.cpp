#include <pt.h>
#include "../../lib/header/Packets.hpp"

#define CHANNEL1 2
#define CHANNEL2 6

static struct pt thread1, thread2;

bool writeLock = false;

uint8_t lastChValueRec[CHANNEL2];

uint8_t readCh(uint8_t channel)
{
	uint8_t value = 0;
	for (int i = 0; i < 4; i++)
	{
		if (digitalRead(channel + i) == HIGH)
		{
			value |= (1 << i);
		}
	}
	return value;
}

void writeCh(uint8_t channel, const uint8_t value)
{
	Serial.println(value);
	for (int i = 0; i < 4; i++)
	{
		digitalWrite(channel + i, (value >> i) & 0x01);
	}
}

void changeChannelDirection(uint8_t channel, uint8_t direction)
{
	for (int i = 0; i < 4; i++)
	{
		pinMode(channel + i, direction);
	}
}

StreamPacket receiveStreamPacket()
{
	StreamPacket packet;

	// Header lesen
	packet.channel = Serial.read();
	packet.dataLength = Serial.read();

	// Daten lesen
	for (int i = 0; i < packet.dataLength; i++)
	{
		while (Serial.available() < 1);
		packet.data.push_back(Serial.read());
	}

	return packet;
}

int processChannel(uint8_t channel, struct pt *pt)
{
	//PT_BEGIN(pt);

	changeChannelDirection(channel, INPUT);
	uint8_t pinReceived = readCh(channel);
	if (pinReceived != lastChValueRec[channel])
	{
		lastChValueRec[channel] = pinReceived;

		//PT_WAIT_UNTIL(pt, writeLock == false);
		writeLock = true;

		Serial.write(channel);
		Serial.write(pinReceived);

		writeLock = false;
	}

	if (Serial.available() > 2)
	{
		//digitalWrite(10, HIGH);
		StreamPacket sp = receiveStreamPacket();
		changeChannelDirection(channel, OUTPUT);
		for (uint8_t byte: sp.data)
		{
			writeCh(channel, byte);
			delayMicroseconds(10);
		}
		//digitalWrite(10, LOW);
	}

	//PT_END(pt)
}

void setup()
{
	Serial.begin(57600);

	//pinMode(10, OUTPUT);
	//pinMode(11, OUTPUT);

	//PT_INIT(&thread1);
	//PT_INIT(&thread2);
}

void loop()
{
	processChannel(CHANNEL1, &thread1);
	processChannel(CHANNEL2, &thread2);
}
