#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>


gpio_pindef_t gpio_pins[] = {
	GPIO_TABLE(GPIO_EXPAND_AS_DEFS)
};


void gpio_hw_config_pin(GPIO_TypeDef *port, uint8_t pin, uint8_t config) {
	uint8_t shift;
	if (pin < 8) {
		shift = pin * 4;
		port->CRL &= ~(0xf << shift);
		port->CRL |= config << shift;
	}
	else {
		shift = (pin - 8) * 4;
		port->CRH &= ~(0xf << shift);
		port->CRH |= (config & 0xf) << shift;
	}
}

void gpio_init(void) {
	uint32_t i;

	// configure clocks
	RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | 
			RCC_APB2ENR_IOPDEN | RCC_APB2ENR_AFIOEN;


	// configure gpios
	for (i = 0; i < GPIO_TOTAL_COUNT; i++) {
		if (gpio_pins[i].flags & GPIO_NOINIT)
			continue;
		else if (gpio_pins[i].flags & GPIO_IN)
			gpio_hw_config_pin(gpio_pins[i].port, gpio_pins[i].pin,
					GPIO_MODE_IN_FLOATING);
		else if (gpio_pins[i].flags & GPIO_IN_PU) {
			gpio_hw_config_pin(gpio_pins[i].port, gpio_pins[i].pin,
					GPIO_MODE_IN_PULLUP);
			gpio_set(i, gpio_pins[i].flags & GPIO_SET);
		}
		else if (gpio_pins[i].flags & GPIO_OUT) {
			gpio_hw_config_pin(gpio_pins[i].port, gpio_pins[i].pin,
					GPIO_MODE_OUT_50MHZ |
					((gpio_pins[i].flags & GPIO_OD) ?
					 GPIO_MODE_OUT_OD :
					 GPIO_MODE_OUT_PP));
			if (i < 16 && gpio_pins[i].flags & GPIO_BKP)
				gpio_set(i, (BKP->DR3 >> i) & 1);
			else
				gpio_set(i, gpio_pins[i].flags & GPIO_SET);
		}
	}
	
	/* Disable the Serial Wire Jtag Debug Port SWJ-DP */
	// #if DEBUG != 1
	// AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
	// AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_DISABLE;
	// #endif
}

void gpio_set(gpio_t pin, bool value) {
	if (pin < 16 && gpio_pins[pin].flags & GPIO_BKP) {
		PWR->CR |= PWR_CR_DBP;
		BKP->DR3 = (BKP->DR3 & ~(1 << pin)) | (value << pin);
		PWR->CR &= ~PWR_CR_DBP;
	}
	if (gpio_pins[pin].flags & GPIO_INV)
		value = !value;
	if (value)
		gpio_pins[pin].port->BSRR = 1 << gpio_pins[pin].pin;
	else
		gpio_pins[pin].port->BRR = 1 << gpio_pins[pin].pin;
}

bool gpio_get(gpio_t pin) {
	bool value;
	if (gpio_pins[pin].flags & GPIO_NOINIT) {
			return false;
	}
	else {
		value = (gpio_pins[pin].port->IDR & (1 << gpio_pins[pin].pin)) >> gpio_pins[pin].pin;
		return (gpio_pins[pin].flags & GPIO_INV) ? !value : value;
	}
}

bool gpio_is_output(gpio_t pin) {
	return (gpio_pins[pin].flags & GPIO_OUT) ? true : false;
}
