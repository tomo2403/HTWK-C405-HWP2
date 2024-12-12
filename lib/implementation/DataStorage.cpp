#include "../header/DataStorage.hpp"

#include <stdexcept>

DataStorage::DataStorage()
{

}

DataStorage::DataStorage(std::vector<uint8_t> dataVector_byte)
    : dataVector_byte(dataVector_byte)
{
    if(dataVector_byte.empty())
    {
        throw std::invalid_argument("The provided vector must not be empty.");
    }
    

    this->dataVectorOffset = 0;
    this->nextNibbleIsHighNibble = true;
}

uint8_t DataStorage::peek_nibble()
{
    if(empty())
    {
        throw std::out_of_range("Attempted to access an element from empty storage.");
    }

    if (nextNibbleIsHighNibble)
    {
        return dataVector_byte.at(dataVectorOffset) >> 4;
    }
    else
    {
        return dataVector_byte.at(dataVectorOffset) & 0x0F;
    }
}

uint8_t DataStorage::pop_nibble()
{
    const uint8_t nextNibble = peek_nibble();

    if (!nextNibbleIsHighNibble)
    {
        dataVectorOffset++;
    }

    nextNibbleIsHighNibble = !nextNibbleIsHighNibble;

    return nextNibble;
}

bool DataStorage::empty()
{
    if(dataVectorOffset >= dataVector_byte.size())
    {
        return true;
    }
    return false;
}