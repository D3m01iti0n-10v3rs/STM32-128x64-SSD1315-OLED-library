/*
 * stm32_ssd1315.h
 *
 *  Created on: Oct 12, 2026
 *      Author: PC
 */

#ifndef INC_STM32_SSD1315_H_
#define INC_STM32_SSD1315_H_

#include <stdio.h>
#include <string.h>

#include "main.h"

// i2c addr
#define SSD1315_ADDR0 			((uint8_t)0x3C)
#define SSD1315_ADDR1 			((uint8_t)0x3D)

// fb
#define SSD1315_FB_SIZE			((uint16_t)1024)

// cmd/data mode sel
#define SSD1315_CTRL_CMD 		((uint8_t)0x00)
#define SSD1315_CTRL_DAT 		((uint8_t)0x40)

// commands
#define SSD1315_CMD_DISP_ON 	((uint8_t)0xAF)
#define SSD1315_CMD_DISP_OFF 	((uint8_t)0xAE)

// struct
typedef struct {
	I2C_HandleTypeDef *hi2c;
	uint8_t addr;

	HAL_StatusTypeDef err;

	uint8_t fb[1024];
} SSD1315_t;

HAL_StatusTypeDef SSD1315_GetPrtStatus(SSD1315_t *oled);

// user functions =======================================================================
SSD1315_t SSD1315_Init(I2C_HandleTypeDef *hi2c, uint8_t addr);

void SSD1315_fb_FillAll(SSD1315_t *oled, uint8_t data);

void SSD1315_SetPixel(SSD1315_t *oled, uint8_t x, uint8_t y);
void SSD1315_ClrPixel(SSD1315_t *oled, uint8_t x, uint8_t y);
void SSD1315_TogPixel(SSD1315_t *oled, uint8_t x, uint8_t y);

void SSD1315_WriteFullFrame(SSD1315_t *oled);

#endif /* INC_STM32_SSD1315_H_ */
