#include "../lib/lib.hpp"

#include "ComInterface.hpp"

using namespace ioManager;

ComInterface driver = ComInterface();
ComManager comManager = ComManager(&driver);

int main()
{
	driver.openCom();

	std::vector<uint8_t> inputData = getBinaryInput();
	comManager.prepareOutgoingQueue(inputData);

	std::vector<uint8_t> outputData = comManager.transfer2Way();
	setBinaryOutput(outputData);
	return 0;
}