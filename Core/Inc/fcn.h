#ifndef INC_FCN_H_
#define INC_FCN_H_

#include "nmea_parse.h"
#include "adc.h"
#include "rtc.h"
#include <stdio.h>

void createFrame(char* buffer, uint8_t maxLen, GPS gps, float voltage);
void createStatusFrame(char* buffer, uint8_t maxLen, float voltage);
uint32_t read_adc();

#endif /* INC_FCN_H_ */
