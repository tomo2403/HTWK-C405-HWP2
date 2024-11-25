#include "../lib/library.cpp"
#include "Serial.hpp"

using namespace ioManager;

int main()
{
	Serial serial;
	serial.openSerialPort();

	// TODO: Implement the main loop here.

	serial.closeSerialPort();
	return 0;
}
