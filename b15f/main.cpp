#include "../lib/lib.hpp"

#include "ComInterface.hpp"

using namespace ioManager;

ComInterface interface = ComInterface();
ComManager comManager = ComManager(&interface);

int main()
{
	interface.openCom();

	std::vector<uint8_t> inputData = getBinaryInput();
	std::vector<uint8_t> outputData = comManager.transfer2Way(inputData);

	setBinaryOutput(outputData);
	interface.closeCom();
	return 0;
}