#ifndef _VARIABLE_H_
#define _VARIABLE_H_

extern void write2variable(const char *name, uint64_t data, bool fDoCallback);
extern uint64_t get_variable(const char *name);
extern void write2variable_by_id(const uint8_t id, uint64_t data, bool fDoCallback);
extern uint64_t get_variable_by_id(const uint8_t id);

/* Variable update handlers */
//extern void uart_extm_variable_update(const uint8_t var_id, uint64_t *value);
//extern void uart_tc_variable_update(const uint8_t var_id, uint64_t *value);
extern void uart_variable_update(const uint8_t var_id, uint64_t *value);
extern void alg_power_variable_update(const uint8_t var_id, uint64_t *value);
extern void alg_usched_variable_update(const uint8_t var_id, uint64_t *value);
extern void alg_adc_cutoff_variable_update(const uint8_t var_id, uint64_t *value);
extern void adc_channels_count_update(const uint8_t var_id, uint64_t *value);


/* Some variable uniq ids to optimize code */
#define VAR_PRNTkeypress	0x01
#define VAR_PRNTuse_led		0x02
#define VAR_USART2baudrate		0x11
#define VAR_USART2wordlen		0x12
#define VAR_USART2stopbits		0x13
#define VAR_USART2parity		0x14
#define VAR_USART3baudrate	0x21
#define VAR_USART3wordlen		0x22
#define VAR_USART3stopbits	0x23
#define VAR_USART3parity		0x24
#define VAR_alg_power		0x03
#define VAR_alg_usched		0x04
#define VAR_alg_adc_cutoff	0x05
#define VAR_core_clock		0x06
#define VAR_adc_channels	0x07
#define VAR_SWbaudrate		0x31
#define VAR_SWwordlen		0x32
#define VAR_SWstopbits		0x33
#define VAR_SWparity		0x34
#define VAR_USART1baudrate	0x41
#define VAR_USART1wordlen	0x42
#define VAR_USART1stopbits	0x43
#define VAR_USART1parity	0x44

#endif /* _VARIABLE_H_ */
