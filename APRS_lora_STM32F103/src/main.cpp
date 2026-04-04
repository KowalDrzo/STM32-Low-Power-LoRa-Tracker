#include "fcn.h"

#define SLEEP_TIME_S 314

TinyGPSPlus gps;
GpsData gpsData;
HardwareSerial Serial1(PA3, PA2);

void setup() {

    delay(500);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, 1);

    Serial1.begin(9600);
    Serial1.println("$PMTK886,3*2B");

    // TODO test voltage measurement.

    gpsData.fixReady = false;
    while (!gpsData.fixReady) {

        if (Serial1.available()) {

            volatile char c = Serial1.read();
            if(gps.encode(c)) {

                gpsData.lat = gps.location.lat();
                gpsData.lng = gps.location.lng();
                gpsData.alt = gps.altitude.feet();
                gpsData.speed = gps.speed.knots();
                gpsData.sats = gps.satellites.value();

                if (gpsData.alt > 10) {
                    gpsData.fixReady = true;
                }
            }
        }
    }

    loraInit();

    delay(100);
}

void loop() {

    if (Serial1.available()) {

        volatile char c = Serial1.read();
        if(gps.encode(c)) {

            gpsData.lat = gps.location.lat();
            gpsData.lng = gps.location.lng();
            gpsData.alt = gps.altitude.feet();
            gpsData.speed = gps.speed.knots();
            gpsData.sats = gps.satellites.value();

            digitalWrite(LED_PIN, 0);
            String txStr = createFrame(gpsData);
            loraSend(txStr);
            digitalWrite(LED_PIN, 1);

            delay(SLEEP_TIME_S * 1000);
            Serial1.flush();
        }
    }
}
