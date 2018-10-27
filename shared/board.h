#ifndef BOARD_H
#define BOARD_H

#include "board_f130c8.h"

#define EXPAND_AS_ENUM(name, ...) name,
#define EXPAND_AS_DEFS(name, port, pin, flags) {port, pin, flags},

typedef enum gpio_e {
	GPIO_TABLE(EXPAND_AS_ENUM)
	GPIO_TOTAL_COUNT
} gpio_t;

/* to prevent compilation error on pin conflict */
#define GPIO_EXPAND_AS_CHECK(name, port, pin, flags) port ## pin,
enum gpio_check {
	GPIO_TABLE(GPIO_EXPAND_AS_CHECK)
};

#endif /* BOARD_H */
