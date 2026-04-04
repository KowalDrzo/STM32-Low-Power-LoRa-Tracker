#ifndef GPSDATA_H
#define GPSDATA_H

#include <stdint.h>

class GpsData {

public:

    float lat;
    float lng;
    uint16_t alt;
    uint16_t speed;
    uint8_t sats;
    bool fixReady;
};

#endif