#include "fcn.h"

void createFrame(char* buffer, uint8_t maxLen, GPS gps, float voltage) {

	memset(buffer, 0, maxLen);

	uint8_t wide = gps.altitude > 1500 ? 1 : 2;

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
		buffer, "SP3MIK-11>APLT00,WIDE%d-1:!%02d%02d.%02d%c/%03d%02d.%02d%cO%03d/%03d/A=%06d%d.%dV%dS",
		wide,
		lat_deg, lat_min_int, lat_min_frac, gps.latSide,
		lon_deg, lon_min_int, lon_min_frac, gps.lonSide,
		(int)gps.heading, (int)gps.speed_knots,
		(int)alt_ft,
		(int)voltage_int, (int)voltage_frac, gps.satelliteCount
	);
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

void Enter_Standby_s(uint32_t seconds)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    RTC_AlarmTypeDef sAlarm = {0};

    // 🔹 Pobierz czas
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    uint32_t total_seconds =
        time.Hours * 3600 +
        time.Minutes * 60 +
        time.Seconds;

    total_seconds += seconds;

    sAlarm.AlarmTime.Hours   = (total_seconds / 3600) % 24;
    sAlarm.AlarmTime.Minutes = (total_seconds % 3600) / 60;
    sAlarm.AlarmTime.Seconds = total_seconds % 60;

    sAlarm.Alarm = RTC_ALARM_A;

    // 🔴 1. Dezaktywuj alarm
    HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

    // 🔴 2. Wyczyść flagę alarmu RTC
    __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc, RTC_FLAG_ALRAF);

    // 🔴 3. (KLUCZOWE!) Wyczyść EXTI line od RTC alarm
    __HAL_RTC_ALARM_EXTI_CLEAR_FLAG();

    // 🔴 4. Wyczyść flagi wakeup PWR
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

    // 🔹 Ustaw alarm
    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

    // 🔹 Standby
    HAL_PWR_EnterSTANDBYMode();
}
