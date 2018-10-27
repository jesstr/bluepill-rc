#ifndef _EXTI_IRQ_H_
#define _EXTI_IRQ_H_

extern void init_exti_irq_handler(void);
extern void add_exti_irq(gpio_t exti_gpio, void (*handler)(uint8_t pin));
extern void del_exti_irq(gpio_t exti_gpio);
extern bool exti_irq_glitch_pass(void);

#endif /* _EXTI_IRQ_H_ */
