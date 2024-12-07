#include "../lib/lib.hpp"

#include "ComInterface.hpp"

using namespace ioManager;

ComInterface serial = ComInterface();
ComManager comManager = ComManager(&serial);

int main()
{
	serial.openCom();

	std::vector<uint8_t> inputData = getBinaryInput();
	comManager.prepareOutgoingQueue(inputData);

	std::vector<uint8_t> outputData = comManager.transfer2Way();
	setBinaryOutput(outputData);
	return 0;
}