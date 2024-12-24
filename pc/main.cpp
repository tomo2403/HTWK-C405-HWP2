#include <vector>
#include "../lib/lib.hpp"

#include "ComInterface.hpp"
#include "../lib/header/ComManager.hpp"

using namespace ioManager;

int main()
{
	auto com = ComInterface();
	auto comManager = ComManager(&com, getBinaryInput());

	const std::vector<uint8_t> output = comManager.transfer2Way();
	setBinaryOutput(output);

	return 0;
}
