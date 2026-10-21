#include "fcn.h"

void createFrame(char* buffer, uint8_t maxLen, GPS gps, float voltage) {

	memset(buffer, 0, maxLen);

	int lat_deg = (int)gps.latitude;
	int lat_minutes100 = (int)((gps.latitude - lat_deg) * 6000 + 0.5); // minuty * 100
	int lat_min_int = lat_minutes100 / 100;
	int lat_min_frac = lat_minutes100 % 100;

	int lon_deg = (int)gps.longitude;
	int lon_minutes100 = (int)((gps.longitude - lon_deg) * 6000 + 0.5); // minuty * 100
	int lon_min_int = lon_minutes100 / 100;
	int lon_min_frac = lon_minutes100 % 100;

	uint32_t alt_ft = (int)gps.altitude * 3.281;

	uint8_t voltage_int = (int)voltage;
	uint8_t voltage_frac = (int)(voltage * 100) % 100;

	sprintf(
		buffer, "SP3MIK-12>APLT00,WIDE1-1:!%02d%02d.%02d%c/%03d%02d.%02d%cO%03d/%03d/A=%06d%d.%dV%dS",
		lat_deg, lat_min_int, lat_min_frac, gps.latSide,
		lon_deg, lon_min_int, lon_min_frac, gps.lonSide,
		(int)gps.heading, (int)gps.speed_knots,
		(int)alt_ft,
		(int)voltage_int, (int)voltage_frac, gps.satelliteCount
	);
}

void createStatusFrame(char* buffer, uint8_t maxLen, float voltage) {
	uint8_t voltage_int = (uint8_t)voltage;
	uint8_t voltage_frac = (uint8_t)(voltage * 100) % 100;

	snprintf(buffer, maxLen,
		"SP3MIK-12>APLT00:>%u.%02uV",
		(unsigned int)voltage_int, (unsigned int)voltage_frac);
}

uint32_t read_adc() {

    HAL_ADC_Start(&hadc1); // start konwersji

    // czekaj aż skończy (timeout 100 ms)
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK)
    {
        return HAL_ADC_GetValue(&hadc1); // wynik 0–4095 (12 bit)
    }

    return 0; // błąd
}

static volatile uint8_t rtcAlarmFired = 0;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *rtc)
{
    (void)rtc;
    rtcAlarmFired = 1;
}
