#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "usart.h"
#include "common.h"
#include "ring_buf.h"
#include "tinystdio.h"

#include "FreeRTOS.h"
#include "semphr.h"

uint32_t uart_baud_mass[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };

typedef struct uart_s {
	bool enabled;
	unsigned baud;
	USART_TypeDef *addr;
	uint8_t *rx_fifo;
	uint8_t *tx_fifo;
	uint32_t buf_size;
	void (*get_baud)(uint8_t *value);
	rbuf_t rx_buf;
	rbuf_t tx_buf;
	SemaphoreHandle_t rx_buf_sem;
	SemaphoreHandle_t tx_buf_sem;
} uart_t;

#define DEBUG_USART_RBUF_SIZE 256

static uint8_t debug_rx_fifo[DEBUG_USART_RBUF_SIZE];
static uint8_t debug_tx_fifo[DEBUG_USART_RBUF_SIZE];

#define FIN { 0 }, { 0 }, 0, 0

static uart_t uarts[] = {
	{
		true,
		115200,
		USART1,
		debug_rx_fifo,
		debug_tx_fifo,
		DEBUG_USART_RBUF_SIZE,
		NULL,
		FIN
	}, {
		false
	}, {
		false
	}
};
#undef FIN


void _putc(void *p, char c);
void _putchar(uint8_t c);

void uart_hw_init(USART_TypeDef *uart, uint32_t baud, uint16_t wordlen, uint16_t parity, uint16_t stop) {
	USART_InitTypeDef USART_InitStructure;
	uint16_t wordlen_tmp;

	if (wordlen == 8)
		wordlen_tmp = USART_WordLength_8b;
	if (wordlen == 9)
		wordlen_tmp = USART_WordLength_9b;

	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = wordlen_tmp;
	USART_InitStructure.USART_StopBits = stop;
	USART_InitStructure.USART_Parity = parity;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_DeInit(uart);

	if (uart == USART1) {
		RCC->APB2ENR |= RCC_APB2Periph_USART1;
		NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x07, 0));
		NVIC_EnableIRQ(USART1_IRQn);
	}

	if (uart == USART2) {
		RCC->APB1ENR |= RCC_APB1Periph_USART2;
		NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x06, 0));
		NVIC_EnableIRQ(USART2_IRQn);
	}

    if (uart == USART3) {
        RCC->APB1ENR |= RCC_APB1Periph_USART3;
        NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x06, 0));
        NVIC_EnableIRQ(USART3_IRQn);
    }

	USART_Init(uart, &USART_InitStructure);
	USART_Cmd(uart, ENABLE);
	USART_ITConfig(uart, USART_IT_RXNE, ENABLE);
	USART_ITConfig(uart, USART_IT_TXE, DISABLE);
}

void uart_irq_enable(uint8_t indx_uarts)
{
	USART_TypeDef *uart = uarts[indx_uarts].addr;

	if (uart == USART1) {
		NVIC_EnableIRQ(USART1_IRQn);
	}
	else if (uart == USART2) {
		NVIC_EnableIRQ(USART2_IRQn);
	} 
	else if (uart == USART3) {
        NVIC_EnableIRQ(USART3_IRQn);
    }
}

void uart_init(void) {
	for (unsigned char i = 0; i < array_len(uarts); ++i) {
		if (!uarts[i].enabled) continue;
		uarts[i].rx_buf_sem = xSemaphoreCreateCounting(uarts[i].buf_size, 0);
		uarts[i].tx_buf_sem = xSemaphoreCreateCounting(uarts[i].buf_size, (uarts[i].buf_size-1));
		rbuf8_init(&uarts[i].tx_buf, uarts[i].tx_fifo, uarts[i].buf_size);
		rbuf8_init(&uarts[i].rx_buf, uarts[i].rx_fifo, uarts[i].buf_size);
		uart_hw_init(uarts[i].addr, uarts[i].baud, 8, USART_Parity_No, USART_StopBits_1);
	}
	init_printf(NULL, _putc);
}

void uart_putchar(uint8_t indx_uarts, uint8_t byte) {
	if (isInterrupt()) {
		if (xSemaphoreTakeFromISR(uarts[indx_uarts].tx_buf_sem, NULL) != pdTRUE) {
			return;
		}
	} else {
		xSemaphoreTake(uarts[indx_uarts].tx_buf_sem, portMAX_DELAY);
	}
	rbuf8_put(&uarts[indx_uarts].tx_buf, byte);
	uarts[indx_uarts].addr->CR1 |= USART_CR1_TXEIE;
}

int uart_send_block(uint8_t indx_uarts, void *data, uint8_t len) {
	int i = 0;
	uint8_t s_temp = 0;
	
	while (i < len) {
		if (isInterrupt()) {
			if (xSemaphoreTakeFromISR(uarts[indx_uarts].tx_buf_sem, NULL) != pdTRUE) {
				return 0;
			}
		} else {
			xSemaphoreTake(uarts[indx_uarts].tx_buf_sem, portMAX_DELAY);
		}
		s_temp = ((uint8_t *)data)[i++];
		rbuf8_put(&uarts[indx_uarts].tx_buf, s_temp);
			
	}
	uarts[indx_uarts].addr->CR1 |= USART_CR1_TXEIE;
	return i;
}

int uart_getchar(uint8_t indx_uarts, unsigned timeout) {
	uint8_t res;
	if (isInterrupt()) {
		if (xSemaphoreTakeFromISR(uarts[indx_uarts].rx_buf_sem, NULL) != pdTRUE) {
			return -1;
		}
	} else {
		res = xSemaphoreTake(uarts[indx_uarts].rx_buf_sem, (TickType_t)timeout);
	}
	if (res == pdFALSE) {
		return -1;
	}
	rbuf8_get(&uarts[indx_uarts].rx_buf, &res);
	return res;
}

void _putchar(uint8_t c) {
#ifdef SWOTRACE
    ITM_SendChar(c);
#else
	uart_putchar(DEBUG_USART, c);
#endif
}

void _putc(void* p, char c) {
	(void)p;
	_putchar(c);
}

static void common_irq_handler(uint8_t indx_uarts)
{
	uint8_t c = 0;

	static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if ((uarts[indx_uarts].addr->SR & USART_SR_ORE)) {
		c = uarts[indx_uarts].addr->DR;
	}
	if (USART_GetITStatus(uarts[indx_uarts].addr, USART_IT_TXE) != RESET) {
		if(rbuf8_get(&uarts[indx_uarts].tx_buf, &c)) {
			uarts[indx_uarts].addr->DR = (uint16_t)c;
			xSemaphoreGiveFromISR(uarts[indx_uarts].tx_buf_sem, &xHigherPriorityTaskWoken);
		}
		else {
			uarts[indx_uarts].addr->CR1 &= ~USART_CR1_TXEIE;
			uarts[indx_uarts].addr->CR1 |= USART_CR1_RXNEIE;
		}
		USART_ClearITPendingBit(uarts[indx_uarts].addr, USART_IT_TXE);
	}
	if(USART_GetITStatus(uarts[indx_uarts].addr, USART_IT_RXNE) != RESET) {
		c = uarts[indx_uarts].addr->DR;
		if (!rbuf_isfull(&uarts[indx_uarts].rx_buf))
			rbuf8_put(&uarts[indx_uarts].rx_buf, c);
		USART_ClearITPendingBit(uarts[indx_uarts].addr, USART_IT_RXNE);
		xSemaphoreGiveFromISR(uarts[indx_uarts].rx_buf_sem, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void USART1_IRQHandler(void) {
	common_irq_handler(0);
}

void USART2_IRQHandler(void) {
	common_irq_handler(1);
}

void USART3_IRQHandler(void) {
	common_irq_handler(2);
}