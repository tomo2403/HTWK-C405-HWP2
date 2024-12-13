#include "../lib/lib.hpp"

#include "ComInterface.hpp"

using namespace ioManager;

auto interface = ComInterface();
auto comManager = ComManager(&interface);

int main()
{
	interface.openCom();

	const std::vector<uint8_t> inputData = getBinaryInput();
	const std::vector<uint8_t> outputData = comManager.transfer2Way(inputData);

	setBinaryOutput(outputData);
	interface.closeCom();
	return 0;
}