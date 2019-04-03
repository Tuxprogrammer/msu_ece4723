#ifndef __ESOS_SPI_UTILS_H
#define __ESOS_SPI_UTILS_H

#include "revF14.h"

void configSPI1(void);
void writeSPI(uint16_t *pu16_out, uint16_t *pu16_in, uint16_t u16_cnt);
void write_DAC(uint16_t u16_data);


#endif