#include "../lib/header/IoManagerB15F.hpp"

int main(int, char**){
	B15F& drv = B15F::getInstance();

	drv.setRegister(&DDRA, 0xe5);
	std::cout << std::bitset<8>(drv.getRegister(&DDRA)) << std::endl;
}
