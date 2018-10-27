#ifndef GPIO_H
#define GPIO_H

#include "common.h"
#include <stdbool.h>
#include <stdint.h>

/* Glitch in us */
#define GLITCH_TIMEOUT 10000

#define GPIO_IN		0x01
#define GPIO_IN_PU	0x02
#define GPIO_OUT	0x04
#define GPIO_SET	0x08
#define GPIO_INV	0x10
#define GPIO_OD		0x20
#define GPIO_BKP	0x40
#define GPIO_NOINIT	0x80

typedef struct {
	GPIO_TypeDef *port;
	uint8_t pin;
	uint8_t flags;
} gpio_pindef_t;

#include "board.h"

#define GPIO_MODE_IN_ANALOG		((0 << 2) | 0)
#define GPIO_MODE_IN_FLOATING		((1 << 2) | 0)
#define GPIO_MODE_IN_PULLUP		((2 << 2) | 0)
#define GPIO_MODE_OUT_10MHZ		1
#define GPIO_MODE_OUT_2MHZ		2
#define GPIO_MODE_OUT_50MHZ		3
#define GPIO_MODE_OUT_PP		(0 << 2)
#define GPIO_MODE_OUT_OD		(1 << 2)
#define GPIO_MODE_ALT_PP		(2 << 2)
#define GPIO_MODE_ALT_OD		(3 << 2)

extern void gpio_init(void);
extern void gpio_set(gpio_t pin, bool value);
extern bool gpio_get(gpio_t pin);
extern bool gpio_is_output(gpio_t pin);
extern void gpio_hw_config_pin(GPIO_TypeDef *port, uint8_t pin, uint8_t config);

#endif /* GPIO_H */
