#include "fcn.h"

void loraInit() {

    SPI.begin();
    LoRa.setPins(LORA_CS_PIN, LORA_RES_PIN, LORA_D0_PIN);
    LoRa.setSignalBandwidth(125E3);

    int ret = LoRa.begin(433775000);
    delay(ret);

    LoRa.setTimeout(100);
    LoRa.setTxPower(12);
    LoRa.enableCrc();

    LoRa.setSpreadingFactor(12);
    LoRa.setCodingRate4(5);
}

void loraSend(String txString) {

    LoRa.beginPacket();
    LoRa.write('<');
    LoRa.write(0xFF);
    LoRa.write(0x01);
    LoRa.write((const uint8_t *)txString.c_str(), txString.length());
    LoRa.endPacket();

    LoRa.sleep();
}

String create_lat_aprs(double lat) {
    char str[20];
    char n_s = 'N';
    if (lat < 0) {
        n_s = 'S';
    }
    /*lat = std::abs(lat);
    sprintf(str, "%02d%05.2f%c", (int)lat, (lat - (double)((int)lat)) * 60.0, n_s);*/

    int deg = (int)lat;
    int minutes100 = (int)((lat - deg) * 6000 + 0.5); // minuty * 100
    int min_int = minutes100 / 100;
    int min_frac = minutes100 % 100;

    sprintf(str, "%02d%02d.%02d%c", deg, min_int, min_frac, n_s);

    String lat_str(str);
    return lat_str;
}

String create_long_aprs(double lng) {
    char str[20];
    char e_w = 'E';
    if (lng < 0) {
        e_w = 'W';
    }
    /*lng = std::abs(lng);
    sprintf(str, "%03d%05.2f%c", (int)lng, (lng - (double)((int)lng)) * 60.0, e_w);*/

    int deg = (int)lng;
    int minutes100 = (int)((lng - deg) * 6000 + 0.5); // minuty * 100
    int min_int = minutes100 / 100;
    int min_frac = minutes100 % 100;

    sprintf(str, "%03d%02d.%02d%c", deg, min_int, min_frac, e_w);

    String lng_str(str);
    return lng_str;
}

String createFrame(GpsData gpsData) {

    String latString = create_lat_aprs(gpsData.lat);
    String lngString = create_long_aprs(gpsData.lng);

    char altString[10], speedString[20];
    sprintf(altString, "%06d", gpsData.alt);
    sprintf(speedString, "000/%03d", gpsData.speed);

    String frame = "SP3MIK-11>APLT00,WIDE";

    if (gpsData.alt > 3000) frame += String(1);
    else frame += String(2);

    frame += "-1:!";
    frame += latString + "/" + lngString; //5106.57N/01703.45E
    frame += "O";
    frame += speedString;
    frame += "/A=";
    frame += altString; //000390
    frame += String(getVoltage(), 2) + "V";
    frame += String(gpsData.sats) + "S";

    return frame;
}

void goToSleep(uint16_t seconds) {

    __HAL_RCC_PWR_CLK_ENABLE();

    // Wyczyść flagę wakeup
    PWR->CR |= PWR_CR_CWUF;

    uint32_t now = (RTC->CNTH << 16) | RTC->CNTL;

    // ustaw alarm
    RTC->ALRH = (now + seconds >> 16) & 0xFFFF;
    RTC->ALRL = now + seconds & 0xFFFF;

    // standby
    PWR->CR |= PWR_CR_PDDS;
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    __WFI();
}

float getVoltage() {

    float voltage = 0.0;
    uint16_t adcValue = analogRead(V_SENSE_PIN);
    if (adcValue > 0) {
        voltage = (adcValue / 4095.0) * 3.3 * 3;
    }

    return voltage;
}
