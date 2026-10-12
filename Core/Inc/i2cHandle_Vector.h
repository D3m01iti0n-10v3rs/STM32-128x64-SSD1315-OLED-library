/*
 * i2cHandle_Vector.h
 *
 *  Created on: Oct 12, 2026
 *      Author: PC
 */

#ifndef INC_I2CHANDLE_VECTOR_H_
#define INC_I2CHANDLE_VECTOR_H_

#include <stddef.h>
#include <stdlib.h>

#include "main.h"

typedef struct {
    I2C_HandleTypeDef *handle; // Pointer to dynamically allocated array
    size_t size;               // Current number of items stored
    size_t capacity;           // Total allocated capacity (in items)
} i2cHandle_Vector;

// Function prototypes
void i2cHVec_Init(i2cHandle_Vector *vector);
uint8_t i2cHVec_Push(i2cHandle_Vector *vector, I2C_HandleTypeDef handle);
void i2cHVec_Free(i2cHandle_Vector *vector);
uint8_t i2cHVec_Contains(const i2cHandle_Vector *vector, I2C_HandleTypeDef handle);

#endif /* INC_I2CHANDLE_VECTOR_H_ */
