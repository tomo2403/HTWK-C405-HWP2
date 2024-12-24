#include "../lib/lib.hpp"

#include "ComInterface.hpp"

using namespace ioManager;

auto com = ComInterface();
//auto comManager = ComManager(&interface);

int main()
{
	com.openCom();

	std::vector<uint8_t> inputData = getBinaryInput();
	//const std::vector<uint8_t> outputData = comManager.transfer2Way(inputData);

	//setBinaryOutput(outputData);
	com.closeCom();
	return 0;
}