/*
 * i2cHandle_Vector.c
 *
 *  Created on: Oct 12, 2026
 *      Author: PC
 */

#include "i2cHandle_Vector.h"

void i2cHVec_Init(i2cHandle_Vector *vector) {
    if (!vector) return;
    vector->handle = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

uint8_t i2cHVec_Push(i2cHandle_Vector *vector, I2C_HandleTypeDef handle) {
    if (!vector) return 0;

    if (vector->size >= vector->capacity) {
        size_t new_capacity = (vector->capacity == 0) ? 4 : vector->capacity * 2;

        // Multiply capacity by sizeof element to get total byte count
        I2C_HandleTypeDef *new_handle = realloc(vector->handle, new_capacity * sizeof(I2C_HandleTypeDef));
        if (!new_handle) {
            return 0; // Allocation failed
        }

        vector->handle = new_handle;
        vector->capacity = new_capacity;
    }

    vector->handle[vector->size++] = handle;
    return 1;
}

void i2cHVec_Free(i2cHandle_Vector *vector) {
    if (!vector) return;

    free(vector->handle);
    vector->handle = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

uint8_t i2cHVec_Contains(const i2cHandle_Vector *vector, I2C_HandleTypeDef handle) {
    if (!vector || !vector->handle) return 0;

    for (size_t i = 0; i < vector->size; i++) {
        // Compare hardware instances (e.g., I2C1 == I2C1)
        if (vector->handle[i].Instance == handle.Instance) {
            return 1; // Found match
        }
    }
    return 0; // Not found
}
