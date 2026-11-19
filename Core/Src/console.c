#include "console.h"
#include "main.h"

void console_init(void) {
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	USART2->BRR = 80000000 / SERIAL_MONITOR;
	USART2->CR1 |= (USART_CR1_TE | USART_CR1_UE);
}

int __io_putchar(int ch) {
	while ((USART2->ISR & USART_ISR_TXE) == 0)
		;
	USART2->TDR = (uint8_t) ch;
	return ch;
}
