#include "LoRa.h"

#define REG_OP_MODE        0x01
#define REG_FRF_MSB        0x06
#define REG_FRF_MID        0x07
#define REG_FRF_LSB        0x08
#define REG_PA_CONFIG      0x09
#define REG_MODEM_CONFIG1  0x1D
#define REG_MODEM_CONFIG2  0x1E
#define REG_MODEM_CONFIG3  0x26
#define REG_PAYLOAD_LENGTH 0x22
#define REG_FIFO           0x00
#define REG_FIFO_TX_BASE   0x0E
#define REG_FIFO_ADDR_PTR  0x0D
#define REG_IRQ_FLAGS      0x12

#define REG_FIFO_TX_BASE_ADDR    0x0e
#define REG_FIFO_RX_BASE_ADDR    0x0f
#define REG_LNA                  0x0c
#define PA_BOOST                 0x80
#define REG_DETECTION_OPTIMIZE   0x31
#define REG_DETECTION_THRESHOLD  0x37

void LoRa_WriteReg(uint8_t addr, uint8_t value)
{
    uint8_t data[2];
    data[0] = addr | 0x80;
    data[1] = value;

    HAL_GPIO_WritePin(LORA_CS_GPIO_Port, LORA_CS_Pin, 0);
    HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LORA_CS_GPIO_Port, LORA_CS_Pin, 1);
}

uint8_t LoRa_ReadReg(uint8_t addr)
{
    uint8_t tx[2], rx[2];

    tx[0] = addr & 0x7F;
    tx[1] = 0x00;

    HAL_GPIO_WritePin(LORA_CS_GPIO_Port, LORA_CS_Pin, 0);
    HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LORA_CS_GPIO_Port, LORA_CS_Pin, 1);

    return rx[1];
}

bool LoRa_Init(uint32_t frequency)
{
    // Sleep + LoRa mode
    LoRa_WriteReg(REG_OP_MODE, 0x80);
    HAL_Delay(10);

    // set frequency
	LoRa_SetFrequency(frequency);

	// set base addresses
	LoRa_WriteReg(REG_FIFO_TX_BASE_ADDR, 0);
	LoRa_WriteReg(REG_FIFO_RX_BASE_ADDR, 0);

	// set LNA boost
	LoRa_WriteReg(REG_LNA, LoRa_ReadReg(REG_LNA) | 0x03);

	// set auto AGC
	LoRa_WriteReg(REG_MODEM_CONFIG3, 0x0C);

	// set output power to 14 dBm
	LoRa_WriteReg(REG_PA_CONFIG, 0x8C);

    // Standby
    LoRa_WriteReg(REG_OP_MODE, 0x81);

    uint8_t version = LoRa_ReadReg(0x42);
    return version == 0x12;
}

void LoRa_SetFrequency(uint32_t frequency)
{
	uint32_t frf = ((uint64_t)frequency << 19) / 32000000;

	LoRa_WriteReg(REG_FRF_MSB, (uint8_t)(frf >> 16));
	LoRa_WriteReg(REG_FRF_MID, (uint8_t)(frf >> 8));
	LoRa_WriteReg(REG_FRF_LSB, (uint8_t)(frf >> 0));
}

void LoRa_SetModemConfig()
{
	// bw
	int bw = 7;
	LoRa_WriteReg(REG_MODEM_CONFIG1, (LoRa_ReadReg(REG_MODEM_CONFIG1) & 0x0f) | (bw << 4));

	// sf
	int sf = 12;
	LoRa_WriteReg(REG_DETECTION_OPTIMIZE, 0xc3);
	LoRa_WriteReg(REG_DETECTION_THRESHOLD, 0x0a);
	LoRa_WriteReg(REG_MODEM_CONFIG2, (LoRa_ReadReg(REG_MODEM_CONFIG2) & 0x0f) | ((sf << 4) & 0xf0));

	// cr
	int cr = 5 - 4;
	LoRa_WriteReg(REG_MODEM_CONFIG1, (LoRa_ReadReg(REG_MODEM_CONFIG1) & 0xf1) | (cr << 1));

	// crc
	LoRa_WriteReg(REG_MODEM_CONFIG2, LoRa_ReadReg(REG_MODEM_CONFIG2) | 0x04);
}

void LoRa_Send(char *data, uint8_t len)
{

    // Standby
	LoRa_WriteReg(REG_OP_MODE, 0x81);

	LoRa_WriteReg(REG_MODEM_CONFIG1, LoRa_ReadReg(REG_MODEM_CONFIG1) & 0xfe);
    // FIFO reset
    LoRa_WriteReg(0x0D, 0x00);
    LoRa_WriteReg(0x22, 0x00);

    int currentLength = LoRa_ReadReg(0x22);

      // check size
      if ((currentLength + len) > 255) {
        len = 255 - currentLength;
      }

      // write data
      for (size_t i = 0; i < len; i++) {
    	  LoRa_WriteReg(REG_FIFO, data[i]);
      }

      // update length
      LoRa_WriteReg(0x22, currentLength + len);

    // TX mode
    LoRa_WriteReg(REG_OP_MODE, 0x83);

    // Wait TX done
    int i = 0;
    while ((LoRa_ReadReg(REG_IRQ_FLAGS) & 0x08) == 0) {
    	i++;
    	HAL_Delay(1);
    }

    // Clear IRQ
    LoRa_WriteReg(REG_IRQ_FLAGS, 0x08);
}

void LoRa_Sleep()
{
    LoRa_WriteReg(REG_OP_MODE, 0x80);
}

void LoRa_config_polifloater() {

	LoRa_WriteReg(0x1d,0x70);
	LoRa_WriteReg(0x26,0x0);
	LoRa_WriteReg(0x1,0x80);
	LoRa_WriteReg(0x6,0x6c);
	LoRa_WriteReg(0x7,0x71);
	LoRa_WriteReg(0x8,0x99);
	LoRa_WriteReg(0xe,0x0);
	LoRa_WriteReg(0xf,0x0);
	LoRa_WriteReg(0xc,0x23);
	LoRa_WriteReg(0x26,0x4);
	LoRa_WriteReg(0x4d,0x84);
	LoRa_WriteReg(0xb,0x2b);
	LoRa_WriteReg(0x9,0x8f);
	LoRa_WriteReg(0x1,0x81);

	HAL_Delay(100);

	LoRa_WriteReg(0x4d,0x84);
	LoRa_WriteReg(0xb,0x2b);
	LoRa_WriteReg(0x9,0x8a);
	LoRa_WriteReg(0x1e,0x74);
	LoRa_WriteReg(0x1,0x80);
	LoRa_WriteReg(0x1,0x81);
	LoRa_WriteReg(0x6,0x6c);
	LoRa_WriteReg(0x7,0x71);
	LoRa_WriteReg(0x8,0x99);
	LoRa_WriteReg(0x31,0xc3);
	LoRa_WriteReg(0x37,0xa);
	LoRa_WriteReg(0x1e,0xc4);
	LoRa_WriteReg(0x26,0xc);
	LoRa_WriteReg(0x1d,0x72);
}
