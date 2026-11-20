#include "vl53l1_platform.h"
#include "i2c.h"
#include "main.h"

int8_t VL53L1_WriteMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	if (count > 253) {
		return -1;
	}
	return (int8_t)i2c_write((uint8_t)dev, index, pdata, (uint8_t)count);
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	if (count > 255) {
		return -1;
	}
	return (int8_t)i2c_read((uint8_t)dev, index, pdata, (uint8_t)count);
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
	return VL53L1_WriteMulti(dev, index, &data, 1);
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
	uint8_t bytes[2];
	bytes[0] = (uint8_t)(data >> 8);
	bytes[1] = (uint8_t)data;
	return VL53L1_WriteMulti(dev, index, bytes, 2);
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
	uint8_t bytes[4];
	bytes[0] = (uint8_t)(data >> 24);
	bytes[1] = (uint8_t)(data >> 16);
	bytes[2] = (uint8_t)(data >> 8);
	bytes[3] = (uint8_t)data;
	return VL53L1_WriteMulti(dev, index, bytes, 4);
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
	return VL53L1_ReadMulti(dev, index, data, 1);
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
	uint8_t bytes[2];
	int8_t rc = VL53L1_ReadMulti(dev, index, bytes, 2);
	if (rc == 0) {
		*data = (uint16_t)((bytes[0] << 8) | bytes[1]);
	}
	return rc;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	uint8_t bytes[4];
	int8_t rc = VL53L1_ReadMulti(dev, index, bytes, 4);
	if (rc == 0) {
		*data = ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16)
				| ((uint32_t)bytes[2] << 8) | bytes[3];
	}
	return rc;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms) {
	(void)dev;
	HAL_Delay((uint32_t)wait_ms);
	return 0;
}
