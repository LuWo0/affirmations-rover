#include "i2c.h"
#include "main.h"

#include <stdio.h>

void i2c_init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

	// GPIOB alt function
	GPIOB->MODER &= ~(3 << (8 * 2));
	GPIOB->MODER |= (2 << (8 * 2));

	GPIOB->MODER &= ~(3 << (9 * 2));
	GPIOB->MODER |= (2 << (9 * 2));

	GPIOB->OTYPER |= ((1 << 8) | (1 << 9));

	GPIOB->AFR[1] &= ~(0xF << 0);
	GPIOB->AFR[1] |= (4 << 0);
	GPIOB->AFR[1] &= ~(0xF << 4);
	GPIOB->AFR[1] |= (4 << 4);

	I2C1->TIMINGR = 0x10909CEC;
	I2C1->CR1 |= I2C_CR1_PE;

}
void i2c_scan(void) {
	for (uint8_t address = 0x08; address <= 0x77; address++) {
		I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;

		if ((I2C1->ISR & I2C_ISR_TXE) != 0) {
			I2C1->TXDR = 0;
		}

		I2C1->CR2 = (address << 1) | (1U << I2C_CR2_NBYTES_Pos)
				| I2C_CR2_AUTOEND | I2C_CR2_START;

		uint32_t spins = 100000;
		while ((I2C1->ISR & I2C_ISR_STOPF) == 0 && spins > 0) {
			spins--;
		}

		if (spins > 0 && (I2C1->ISR & I2C_ISR_NACKF) == 0) {
			printf("0x%02X\r\n", address);
		}

		I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
	}
}

static int i2c_wait(uint32_t flag) {
	uint32_t spins = 100000;
	while ((I2C1->ISR & flag) == 0 && spins > 0) {
		if ((I2C1->ISR & I2C_ISR_NACKF) != 0) {
			return -1;
		}
		spins--;
	}
	if (spins == 0) {
		return -1;
	}
	return 0;
}

static int i2c_finish(int failed) {
	uint32_t spins = 100000;
	while ((I2C1->ISR & I2C_ISR_STOPF) == 0 && spins > 0) {
		spins--;
	}
	int rc = 0;
	if (failed || spins == 0 || (I2C1->ISR & I2C_ISR_NACKF) != 0) {
		rc = -1;
	}
	I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
	return rc;
}

int i2c_write(uint8_t address, uint16_t index, const uint8_t *bytes,
		uint8_t count) {
	if (count > 253 || (count > 0 && bytes == 0)) {
		return -1;
	}

	I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
	I2C1->CR2 = ((uint32_t) address << 1)
			| ((uint32_t) (count + 2) << I2C_CR2_NBYTES_Pos) | I2C_CR2_AUTOEND
			| I2C_CR2_START;

	if (i2c_wait(I2C_ISR_TXIS) != 0) {
		return i2c_finish(1);
	}
	I2C1->TXDR = (uint8_t) (index >> 8);

	if (i2c_wait(I2C_ISR_TXIS) != 0) {
		return i2c_finish(1);
	}
	I2C1->TXDR = (uint8_t) index;

	for (uint8_t i = 0; i < count; i++) {
		if (i2c_wait(I2C_ISR_TXIS) != 0) {
			return i2c_finish(1);
		}
		I2C1->TXDR = bytes[i];
	}

	return i2c_finish(0);
}

int i2c_read(uint8_t address, uint16_t index, uint8_t *buffer, uint8_t count) {
	if (count == 0 || buffer == 0) {
		return -1;
	}

	I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;
	I2C1->CR2 = ((uint32_t) address << 1) | (2U << I2C_CR2_NBYTES_Pos)
			| I2C_CR2_START;

	if (i2c_wait(I2C_ISR_TXIS) != 0) {
		return i2c_finish(1);
	}
	I2C1->TXDR = (uint8_t) (index >> 8);

	if (i2c_wait(I2C_ISR_TXIS) != 0) {
		return i2c_finish(1);
	}
	I2C1->TXDR = (uint8_t) index;

	if (i2c_wait(I2C_ISR_TC) != 0) {
		return i2c_finish(1);
	}

	I2C1->CR2 = ((uint32_t) address << 1)
			| ((uint32_t) count << I2C_CR2_NBYTES_Pos) | I2C_CR2_RD_WRN
			| I2C_CR2_AUTOEND | I2C_CR2_START;

	for (uint8_t i = 0; i < count; i++) {
		if (i2c_wait(I2C_ISR_RXNE) != 0) {
			return i2c_finish(1);
		}
		buffer[i] = (uint8_t) I2C1->RXDR;
	}

	return i2c_finish(0);
}
