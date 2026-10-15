/*
 * stm32_ssd1315.c
 *
 *  Created on: Oct 12, 2026
 *      Author: PC
 */

#include "stm32_ssd1315.h"
#include "stm32_ssd1315_fonts.h"

// get/set functions =====================================================================
HAL_StatusTypeDef SSD1315_GetPrtStatus(SSD1315_t *oled){
	if (HAL_I2C_GetState(oled->hi2c) != HAL_I2C_STATE_READY) return HAL_BUSY;
	return HAL_OK;
}

// helpers
inline static uint8_t SSD1315_FindPageY(uint8_t y){
	return y / 8;
}

inline static uint8_t SSD1315_FindIndexY(uint8_t y){
	return y % 8;
}

inline static uint16_t SSD1315_FindByteIndex(uint8_t x, uint8_t y){
	return (x + ((y / 8) * 128));
}

// init =================================================================================
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

// modify fb ============================================================================
void SSD1315_fb_FillAll(SSD1315_t *oled, uint8_t data){
	oled->err = HAL_OK;

	memset(oled->fb, data, SSD1315_FB_SIZE);
}

void SSD1315_SetPixel(SSD1315_t *oled, uint8_t x, uint8_t y){
	oled->err = HAL_OK;

    uint16_t byte_i = x + ((y / 8) * 128);
    uint8_t bit_o = y % 8;
    oled->fb[byte_i] |= ((uint8_t) 1 << bit_o);
}

void SSD1315_ClrPixel(SSD1315_t *oled, uint8_t x, uint8_t y){
	oled->err = HAL_OK;

    uint16_t byte_i = x + ((y / 8) * 128);
    uint8_t bit_o = y % 8;
    oled->fb[byte_i] &= ~((uint8_t) 1 << bit_o);
}

void SSD1315_TogPixel(SSD1315_t *oled, uint8_t x, uint8_t y){
	oled->err = HAL_OK;

    uint16_t byte_i = x + ((y / 8) * 128);
    uint8_t bit_o = y % 8;
    oled->fb[byte_i] ^= ((uint8_t) 1 << bit_o);
}

void SSD1315_DrwLine_H(SSD1315_t *oled, int8_t x0, int8_t y0, int8_t x1, int8_t y1){
	oled->err = HAL_OK;

	if (x0 == x1){ // vertical line
		if (y0 > y1){
			int8_t temp = y0;
			y0 = y1;
			y1 = temp;
		}
		for (int8_t i = y0; i <= y1; i++) SSD1315_SetPixel(oled, (uint8_t)x0, (uint8_t)i);
		return;
	}

	if (y0 == y1){ // horizontal line
		if (x0 > x1){
			int8_t temp = x0;
			x0 = x1;
			x1 = temp;
		}
		for (int8_t i = x0; i <= x1; i++) SSD1315_SetPixel(oled, (uint8_t)i, (uint8_t)y0);
		return;
	}

	// diagonal line
	int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
	int16_t dy = (y1 > y0) ? -(y1 - y0) : -(y0 - y1);
	int16_t err = dx + dy;

	int8_t sx = (x0 < x1) ? 1 : -1;
	int8_t sy = (y0 < y1) ? 1 : -1;

	while(1){
		SSD1315_SetPixel(oled, (uint8_t) x0, (uint8_t) y0);
		if (x0 == x1 && y0 == y1) break;

		int16_t e2 = 2 * err;

		if (e2 >= dy){
			err += dy;
			x0 += sx;
		}

		if (e2 <= dx){
			err += dx;
			y0 += sy;
		}
	}

	return;
}

void SSD1315_DrwLine_L(SSD1315_t *oled, int8_t x0, int8_t y0, int8_t x1, int8_t y1){
	oled->err = HAL_OK;

	if (x0 == x1){ // vertical line
		if (y0 > y1){
			int8_t temp = y0;
			y0 = y1;
			y1 = temp;
		}
		for (int8_t i = y0; i <= y1; i++) SSD1315_ClrPixel(oled, (uint8_t)x0, (uint8_t)i);
		return;
	}

	if (y0 == y1){ // horizontal line
		if (x0 > x1){
			int8_t temp = x0;
			x0 = x1;
			x1 = temp;
		}
		for (int8_t i = x0; i <= x1; i++) SSD1315_ClrPixel(oled, (uint8_t)i, (uint8_t)y0);
		return;
	}

	// diagonal line
	int16_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
	int16_t dy = (y1 > y0) ? -(y1 - y0) : -(y0 - y1);
	int16_t err = dx + dy;

	int8_t sx = (x0 < x1) ? 1 : -1;
	int8_t sy = (y0 < y1) ? 1 : -1;

	while(1){
		SSD1315_ClrPixel(oled, (uint8_t) x0, (uint8_t) y0);
		if (x0 == x1 && y0 == y1) break;

		int16_t e2 = 2 * err;

		if (e2 >= dy){
			err += dy;
			x0 += sx;
		}

		if (e2 <= dx){
			err += dx;
			y0 += sy;
		}
	}

	return;
}

void SSD1315_FillRect_H(SSD1315_t *oled, uint8_t lft_x, uint8_t top_y, uint8_t w, uint8_t h){

	uint8_t bot_y = top_y + h - 1;
	uint8_t top_p = SSD1315_FindPageY(top_y);
	uint8_t bot_p = SSD1315_FindPageY(bot_y);
	uint8_t top_p_idx = SSD1315_FindIndexY(top_y);
	uint8_t bot_p_idx = SSD1315_FindIndexY(bot_y);

	// fill on single page
	if (top_p == bot_p) {
		uint8_t mask = (0xFF << top_p_idx) & (0xFF >> (7 - bot_p_idx));
		uint16_t offset = (top_p * SSD1315_FB_PSIZE) + lft_x;

		for (uint8_t x = 0; x < w; x++) oled->fb[offset + x] |= mask;

		return;
	}

	// multi pafe fill
	uint8_t top_mask = 0xFF << top_p_idx; // top page
	uint16_t top_offset = (top_p * SSD1315_FB_PSIZE) + lft_x;
	for (uint8_t i = 0; i < w; i++) oled->fb[top_offset + i] |= top_mask;

	for (uint8_t i = top_p + 1; i < bot_p; i++) // middle pages
		memset(oled->fb + (i * SSD1315_FB_PSIZE) + lft_x, 0xFF, w);

	uint8_t bot_mask = 0xFF >> (7 - bot_p_idx); // bot page
	uint16_t bot_offset = (bot_p * SSD1315_FB_PSIZE) + lft_x;
	for (uint8_t i = 0; i < w; i++) oled->fb[bot_offset + i] |= bot_mask;
}

void SSD1315_FillRect_L(SSD1315_t *oled, uint8_t lft_x, uint8_t top_y, uint8_t w, uint8_t h){

	uint8_t bot_y = top_y + h - 1;
	uint8_t top_p = SSD1315_FindPageY(top_y);
	uint8_t bot_p = SSD1315_FindPageY(bot_y);
	uint8_t top_p_idx = SSD1315_FindIndexY(top_y);
	uint8_t bot_p_idx = SSD1315_FindIndexY(bot_y);

	// fill on single page
	if (top_p == bot_p) {
		uint8_t mask = ~((0xFF << top_p_idx) & (0xFF >> (7 - bot_p_idx)));
		uint16_t offset = (top_p * SSD1315_FB_PSIZE) + lft_x;

		for (uint8_t x = 0; x < w; x++) oled->fb[offset + x] &= mask;

		return;
	}

	// multi pafe fill
	uint8_t top_mask = ~(0xFF << top_p_idx); // top page
	uint16_t top_offset = (top_p * SSD1315_FB_PSIZE) + lft_x;
	for (uint8_t i = 0; i < w; i++) oled->fb[top_offset + i] &= top_mask;

	for (uint8_t i = top_p + 1; i < bot_p; i++) // middle pages
		memset(oled->fb + (i * SSD1315_FB_PSIZE) + lft_x, 0x00, w);

	uint8_t bot_mask = ~(0xFF >> (7 - bot_p_idx)); // bot page
	uint16_t bot_offset = (bot_p * SSD1315_FB_PSIZE) + lft_x;
	for (uint8_t i = 0; i < w; i++) oled->fb[bot_offset + i] &= bot_mask;
}

void SSD1315_DrawRect_H(SSD1315_t *oled, uint8_t lft_x, uint8_t top_y, uint8_t w, uint8_t h){
	uint8_t rgt_x = lft_x + w - 1;
	uint8_t bot_y = top_y + h - 1;

	SSD1315_DrwLine_H(oled, lft_x, top_y, rgt_x, top_y);
	SSD1315_DrwLine_H(oled, lft_x, top_y, lft_x, bot_y);
	SSD1315_DrwLine_H(oled, rgt_x, bot_y, lft_x, bot_y);
	SSD1315_DrwLine_H(oled, rgt_x, bot_y, rgt_x, top_y);

	return;
}

void SSD1315_DrawRect_L(SSD1315_t *oled, uint8_t lft_x, uint8_t top_y, uint8_t w, uint8_t h){
	uint8_t rgt_x = lft_x + w - 1;
	uint8_t bot_y = top_y + h - 1;

	SSD1315_DrwLine_L(oled, lft_x, top_y, rgt_x, top_y);
	SSD1315_DrwLine_L(oled, lft_x, top_y, lft_x, bot_y);
	SSD1315_DrwLine_L(oled, rgt_x, bot_y, lft_x, bot_y);
	SSD1315_DrwLine_L(oled, rgt_x, bot_y, rgt_x, top_y);

	return;
}

void SSD1315_DrawChar_H(SSD1315_t *oled, uint8_t lft_x, uint8_t top_y, const char c, SSD1315_font *font){ // x = 1, y = 1
	
	char ascii_conv = c - SSD1315_ASCII_BM_OFFSET;

	uint8_t bot_y = top_y + font->h - 1; // 6
	uint8_t top_p = SSD1315_FindPageY(top_y); // page 0
	uint8_t bot_p = SSD1315_FindPageY(bot_y); // page 0
	uint8_t top_p_idx = SSD1315_FindIndexY(top_y); // 1
	uint8_t bot_p_idx = SSD1315_FindIndexY(bot_y); // 6
	
	if (top_p == bot_p) { // char stays on same page
		uint8_t mask = (0xFF << top_p_idx) & (0xFF >> (7 - bot_p_idx)); // mask = 1111_1110 & 
		uint16_t offset = (top_p * SSD1315_FB_PSIZE) + lft_x; // buffer byte offset

		for (uint8_t x = 0; x < font->w; x++){
			uint8_t text_mask = font->bitmap[x] | mask; // mask bitmap and mask
			oled->fb[offset + x] |= mask;
		}

		return;
	}

	// char spans multiple pages
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
