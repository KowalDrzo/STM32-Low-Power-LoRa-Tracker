#ifndef INC_FCN_H_
#define INC_FCN_H_

#include "nmea_parse.h"
#include "adc.h"
#include "rtc.h"
#include <stdio.h>

void createFrame(char* buffer, uint8_t maxLen, GPS gps, float voltage);
uint32_t read_adc();
void Enter_Standby_s(uint32_t seconds);

#endif /* INC_FCN_H_ */
