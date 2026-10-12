/*
 * stm32_ssd1315.c
 *
 *  Created on: Oct 12, 2026
 *      Author: PC
 */

#include "stm32_ssd1315.h"

// get/set functions =====================================================================
HAL_StatusTypeDef SSD1315_GetPrtStatus(SSD1315_t *oled){
	if (HAL_I2C_GetState(oled->hi2c) != HAL_I2C_STATE_READY) return HAL_BUSY;
	return HAL_OK;
}

// helpers


// user functions =======================================================================
SSD1315_t SSD1315_Init(I2C_HandleTypeDef *hi2c, uint8_t addr){
	SSD1315_t oled = {hi2c, addr, HAL_OK};

	uint8_t cmd_buf[27] = {
		SSD1315_CTRL_CMD,
		0xAE,
		0xD5, 0x80,
		0xA8, 0x3F,
		0xD3, 0x00,
		0x40,
		0x8D, 0x14,
		0x20, 0x00,
		0xA1,
		0xC8,
		0xDA, 0x12,
		0x81, 0x7F,
		0xD9, 0x22,
		0xDB, 0x20,
		0xA4,
		0xA6,
		0x2E,
		0xAF
	};

	uint8_t tries = 3;
	while (HAL_I2C_Master_Transmit(oled.hi2c, oled.addr << 1, cmd_buf, sizeof(cmd_buf), 50) != HAL_OK) { // loop until sucess, store err code
		if (--tries == 0) {
			oled.err = HAL_TIMEOUT;
			break;
		}
	}

	return oled;
}

void SSD1315_fb_FillAll(SSD1315_t *oled, uint8_t data){
	memset(oled->fb, data, SSD1315_FB_SIZE);
}

void SSD1315_WriteFullFrame(SSD1315_t *oled){
	HAL_StatusTypeDef err_code;
	if ((err_code = SSD1315_GetPrtStatus(oled)) != HAL_OK){
		oled->err = err_code;
		return;
	}

	oled->err = HAL_OK;

	uint8_t cmd_buf[7] = {
		SSD1315_CTRL_CMD,	// enter cmd mode
		0x21, 0x00, 0x7F,	// resets the column pointer to 0
		0x22, 0x00, 0x07	// resets the page pointer to 0
	};

	if ((err_code = HAL_I2C_Master_Transmit(oled->hi2c, oled->addr << 1, cmd_buf, sizeof(cmd_buf), 50)) != HAL_OK) {
		oled->err = err_code;
		return;
	}

	if ((err_code = HAL_I2C_Mem_Write_DMA(oled->hi2c, oled->addr << 1, SSD1315_CTRL_DAT, I2C_MEMADD_SIZE_8BIT, oled->fb, SSD1315_FB_SIZE)) != HAL_OK) {
		oled->err = err_code;
		return;
	}

	return;
}
