#ifndef FCN_H
#define FCN_H

#include "pinout.h"
#include <Arduino.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <LoRa.h>
#include <EEPROM.h>
#include "gpsData.h"
#include "stm32f1xx_hal.h"

void loraInit();
void loraSend(String txString);
String createFrame(GpsData gpsData);
void goToSleep(uint16_t seconds);
float getVoltage();

#endif
