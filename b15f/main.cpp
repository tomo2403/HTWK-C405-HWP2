#include <b15f/b15f.h>
#include <iostream>

#include "crc.hpp"

using namespace std;

int main()
{
    B15F& drv = B15F::getInstance();
    drv.setRegister(&DDRA, 0x0F);

    drv.setRegister(&PORTA, 0x05);

//    while (true) {
//        drv.setRegister(&PORTA, 0x00);
//        cout << "0" << endl;
//        //drv.delay_ms(500);
//        drv.setRegister(&PORTA, 0xA0);
//        cout << "1" << endl;
//        //drv.delay_ms(500);
//    }

	return 0;
}
