#ifndef INC_LORA_H_
#define INC_LORA_H_

#include "spi.h"
#include "string.h"
#include "stdbool.h"

bool LoRa_Init(uint32_t frequency);
void LoRa_SetFrequency(uint32_t frequency);
void LoRa_SetModemConfig();
void LoRa_Sleep();
void LoRa_Send(char *data, uint8_t len);

#endif /* INC_LORA_H_ */
