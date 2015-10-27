#ifndef IO_H
#define IO_H

#include "fatfs/ff.h"

void io_init();
void take_screenshot();

extern uint8_t mount_ok;

#endif
