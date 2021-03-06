#include "one_wire.h"
#include "tinystdio.h"

one_wire_device one_wire_devices[10];
u8 one_wire_device_count = 0;

GPIO_TypeDef *gpio;
u16 pin;
TIM_TypeDef *timer;

one_wire_state state;

// global search state
u8 last_mismatch;
u8 last_device_flag;
u8 crc8;
u8 ROM_NO[8];

const unsigned char crc_table[] = {
	0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
	157,195, 33,127,252,162, 64, 30, 95, 1,227,189, 62, 96,130,220,
	35,125,159,193, 66, 28,254,160,225,191, 93, 3,128,222, 60, 98,
	190,224, 2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
	70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89, 7,
	219,133,103, 57,186,228, 6, 88, 25, 71,165,251,120, 38,196,154,
	101, 59,217,135, 4, 90,184,230,167,249, 27, 69,198,152,122, 36,
	248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91, 5,231,185,
	140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
	17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
	175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
	50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
	202,148,118, 40,171,245, 23, 73, 8, 86,180,234,105, 55,213,139,
	87, 9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
	233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
	116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};


static void one_wire_delay_us(unsigned int time) {
	timer->CNT = 0;
	//time -= 3;
	time -= 1;
	while (timer->CNT <= time) {}
}

void one_wire_init_f10x(GPIO_TypeDef *g, u16 p, TIM_TypeDef *t) {
	gpio = g;
	pin = p;
	timer = t;
	state = ONE_WIRE_ERROR;

	/* Enable GPIO clock */
	if (gpio == GPIOA)
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	else if (gpio == GPIOB)
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
	else if (gpio == GPIOC)
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
	else if (gpio == GPIOD)
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
	else
		while(1){} // not implemented

	// Use PD5 as bus master
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(gpio, &GPIO_InitStructure);

	// Setup clock
	if (timer == TIM2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	else if (timer == TIM3)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	else
		while(1){} // not implemented

	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_Prescaler = 16 - 1; // For 16 MHz APB2 timer clock
	TIM_InitStructure.TIM_Period = 10000 - 1; // Update event every 10000 us / 10 ms
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(timer, &TIM_InitStructure);
//	TIM_ITConfig(timer, TIM_IT_Update, ENABLE);
	TIM_Cmd(timer, ENABLE);
}

#if 0
void one_wire_init_f4xx(GPIO_TypeDef *g, u16 p, TIM_TypeDef *t) {
    gpio = g;
    pin = p;
    timer = t;
    state = ONE_WIRE_ERROR;

    /* Enable GPIO clock */
    if (gpio == GPIOA)
      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
    else if (gpio == GPIOB)
      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
    else if (gpio == GPIOC)
      RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
    else
        while(1){} // not implemented

    // Use PC6 as bus master
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Pin = pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(gpio, &GPIO_InitStructure);

    // Setup clock
    if (timer == TIM2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    else if (timer == TIM3)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    else
        while(1){} // not implemented

    TIM_TimeBaseInitTypeDef TIM_InitStructure;
    TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_InitStructure.TIM_Prescaler = 30 - 1; // For 30 MHz APB2 timer clock
    TIM_InitStructure.TIM_Period = 10000 - 1; // Update event every 10000 us / 10 ms
    TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(timer, &TIM_InitStructure);
//  TIM_ITConfig(timer, TIM_IT_Update, ENABLE);
    TIM_Cmd(timer, ENABLE);
}
#endif

void one_wire_init(GPIO_TypeDef *g, u16 p, TIM_TypeDef *t) {
    one_wire_init_f4xx(g, p, t);
    //one_wire_init_f10x(g, p, t);
}

bool one_wire_reset_pulse() {
	// Pull bus down for 500 us (min. 480 us)
	GPIO_ResetBits(gpio, pin);
	one_wire_delay_us(500);
	GPIO_SetBits(gpio, pin);

	// Wait 70 us, bus should be pulled up by resistor and then
	// pulled down by slave (15-60 us after detecting rising edge)
	one_wire_delay_us(70);
	BitAction bit = GPIO_ReadInputDataBit(gpio, pin);
	if (bit == Bit_RESET) {
		GPIO_SetBits(GPIOC, GPIO_Pin_9);
		GPIO_ResetBits(GPIOC, GPIO_Pin_9);
		state = ONE_WIRE_SLAVE_PRESENT;
	} else {
		state = ONE_WIRE_ERROR;
		return false;
	}

	// Wait additional 430 us until slave keeps bus down (total 500 us, min. 480 us)
	one_wire_delay_us(430);
	return true;
}

void one_wire_write_1() {
	// Pull bus down for 15 us
	GPIO_ResetBits(gpio, pin);
	one_wire_delay_us(15);
	GPIO_SetBits(gpio, pin);

	// Wait until end of timeslot (60 us) + 5 us for recovery
	one_wire_delay_us(50);
}

void one_wire_write_0() {
	// Pull bus down for 60 us
	GPIO_ResetBits(gpio, pin);
	one_wire_delay_us(60);
	GPIO_SetBits(gpio, pin);

	// Wait until end of timeslot (60 us) + 5 us for recovery
	one_wire_delay_us(5);
}

void one_wire_write_bit(bool bit) {
	if (bit) {
		one_wire_write_1();
	} else {
		one_wire_write_0();
	}
}

bool one_wire_read_bit() {
	// Pull bus down for 5 us
	GPIO_ResetBits(gpio, pin);
	one_wire_delay_us(5);
	GPIO_SetBits(gpio, pin);

	// Wait 5 us and check bus state
	one_wire_delay_us(5);

	static BitAction bit;
	bit = GPIO_ReadInputDataBit(gpio, pin);
	GPIO_WriteBit(GPIOC, GPIO_Pin_9, bit);

	// Wait until end of timeslot (60 us) + 5 us for recovery
	one_wire_delay_us(55);

	if (bit == Bit_SET) {
		return true;
	} else {
		return false;
	}
}

void one_wire_write_byte(u8 data) {
	u8 i;
	for (i = 0; i < 8; ++i) {
		if ((data >> i) & 1) {
			one_wire_write_1();
		} else {
			one_wire_write_0();
		}
	}
}

u8 one_wire_read_byte() {
	u8 i;
	u8 data = 0;
	bool bit;
	for (i = 0; i < 8; ++i) {
		bit = one_wire_read_bit();
		data |= bit << i;
	}
	return data;
}

void one_wire_reset_crc() {
	crc8 = 0;
}

u8 one_wire_get_crc() {
	return crc8;
}

u8 one_wire_crc(u8 data) {
	crc8 = crc_table[crc8 ^ data];
	return crc8;
}

void one_wire_read_rom() {
	one_wire_reset_pulse();
	one_wire_write_byte(0x33);
	one_wire_read_byte();
	one_wire_read_byte();
	one_wire_read_byte();
	one_wire_read_byte();
	one_wire_read_byte();
	one_wire_read_byte();
	one_wire_read_byte();
	one_wire_read_byte();
}



int one_wire_search()
{
	u8 id_bit_number;
	int last_zero, rom_byte_number, search_result;
	bool id_bit, cmp_id_bit;
	u8 rom_byte_mask, search_direction;

	// initialize for search
	id_bit_number = 1;
	last_zero = 0;
	rom_byte_number = 0;
	rom_byte_mask = 1;
	search_result = 0;
	crc8 = 0;
	// if the last call was not the last one
	if (!last_device_flag)
	{
		// 1-Wire reset
		if (!one_wire_reset_pulse()) {
			// reset the search
			last_mismatch = 0;
			last_device_flag = false;
			return false;
		}
		// issue the search command
		one_wire_write_byte(0xF0);
		// loop to do the search
		do
		{
			// read a bit and its complement
			id_bit = one_wire_read_bit();
			cmp_id_bit = one_wire_read_bit();
			// check for no devices on 1-wire
			if ((id_bit == 1) && (cmp_id_bit == 1))
				break; // no devices

			// all devices coupled have 0 or 1
			if (id_bit != cmp_id_bit)
				search_direction = id_bit; // bit write value for search
			else
			{
				// if this discrepancy if before the Last Discrepancy
				// on a previous next then pick the same as last time
				if (id_bit_number < last_mismatch)
					search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
				else
					// if equal to last pick 1, if not then pick 0
					search_direction = (id_bit_number == last_mismatch);
				// if 0 was picked then record its position in LastZero
				if (search_direction == 0)
				{
					last_zero = id_bit_number;
				}
			}
			// set or clear the bit in the ROM byte rom_byte_number
			// with mask rom_byte_mask
			if (search_direction == 1)
				ROM_NO[rom_byte_number] |= rom_byte_mask;
			else
				ROM_NO[rom_byte_number] &= ~rom_byte_mask;
			// serial number search direction write bit
			one_wire_write_bit(search_direction);
			// increment the byte counter id_bit_number
			// and shift the mask rom_byte_mask
			id_bit_number++;
			rom_byte_mask <<= 1;
			// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
			if (rom_byte_mask == 0)
			{
				one_wire_crc(ROM_NO[rom_byte_number]); // accumulate the CRC
				rom_byte_number++;
				rom_byte_mask = 1;
			}

		}
		while(rom_byte_number < 8); // loop until through all ROM bytes 0-7
		// if the search was successful then
		if (!((id_bit_number < 65) || (crc8 != 0)))
		{
			// search successful so set last_mismatch, last_device_flag, search_result
			last_mismatch = last_zero;
			// check for last device
			if (last_mismatch == 0)
				last_device_flag = true;

			search_result = true;
		}
	}
	// if no device found then reset counters so next 'search' will be like a first
	if (!search_result || !ROM_NO[0])
	{
		last_mismatch = 0;
		last_device_flag = false;
		search_result = false;
	}
	return search_result;
}

bool one_wire_match_rom(one_wire_device device) {
	int i;
	one_wire_reset_pulse();
	one_wire_write_byte(0x55); // Match ROM command
	for (i = 0; i < 8; ++i) {
		one_wire_write_byte(device.address[i]);
	}

	return false;
}

int one_wire_first()
{
	// reset the search state
	last_mismatch = 0;
	last_device_flag = false;
	return one_wire_search();
}

int one_wire_next()
{
	// leave the search state alone
	return one_wire_search();
}

one_wire_device* one_wire_search_rom(u8 *devices) {
	int result, i;
	one_wire_device_count = 0;
	result = one_wire_first();
	char buffer[10];
	while (result)
	{
		one_wire_device device;
		// print device found - CRC, ID, Family
		for (i = 7; i >= 0; i--) {
		    sprintf(buffer, "%02X", ROM_NO[i]);
		    printf("%s", buffer);
		}

		for (i = 7; i >= 0; i--) {
			device.address[i] = ROM_NO[i];
		}

		one_wire_devices[one_wire_device_count++] = device;

		printf("\r\n");
		result = one_wire_next();
	}
	*devices = one_wire_device_count;
	return one_wire_devices;
}
