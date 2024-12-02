#include "B15Sender.hpp"

B15Sender::B15Sender(B15F& drv, Decoder& decoder, Encoder& encoder) : drv(drv), encoder(encoder), decoder(decoder)
{ }