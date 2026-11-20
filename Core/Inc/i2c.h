#ifndef I2C_H
#define I2C_H

#include <stdint.h>

void i2c_init(void);
void i2c_scan(void);
int i2c_write(uint8_t address, uint16_t index, const uint8_t *bytes, uint8_t count);
int i2c_read(uint8_t address, uint16_t index, uint8_t *buffer, uint8_t count);

#endif
