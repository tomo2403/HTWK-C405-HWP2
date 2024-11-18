#pragma once

#include <cstdint>
#include <vector>
#include <iostream>

class ioManager
{
public:
	void send(uint8_t *data[]);

	void receive(uint8_t *data[]);

	static std::vector<uint8_t> getBinaryPipeContent();

	static void printVector(const std::vector<uint8_t> &data);
};

