#ifndef REGISTERCONSTANTS_H
#define REGISTERCONSTANTS_H
const unsigned int RESET_REGISTER = 0x05;
const unsigned int FIRST_REGISTER = 0x08;
const unsigned int REGISTER_COUNT = (1<<10);
const unsigned int UL_TARGET = 1;
const unsigned int CHANNEL_SOURCE = 3;
const unsigned int TRIGGER_REGISTER = 0x04;
const unsigned int DC_SHIFT_REGISTER = 0x05;
const unsigned int PHA_CONTROL_REGISTER = 0x06;
const unsigned int RESET_SPECTRUM_BIT = 0b010;
const unsigned int ACTIVE_SPECTRUM_BIT = 0b001;
const unsigned int DEBUG_SPECTRUM_BIT = 0b100;
#endif // REGISTERCONSTANTS_H
