#include "ds18b20_.h"
#ifdef PRINTF_STDLIB
#include <stdio.h>
#endif
#ifdef PRINTF_CUSTOM
#include "tinystdio.h"
#endif

#include "FreeRTOS.h"
#include "task.h"


u8 precission = 3;
one_wire_device *one_wire_list_of_devices;
u8 one_wire_devices_list_size = 2;

simple_float *temperatures;

void  ds18b20_task(void) {
        /* Run with auto detection sensors on bus: */
        while(1)
        {
            ds18b20_read_temperature_all();
            ds18b20_wait_for_conversion();
            printf("%d---\r\n", ds18b20_get_precission());
            ds18b20_convert_temperature_all();
        }
#if 0
        /* Run without auto detection sensors on bus: 1 bus - 1 sensor */
        while(1)
        {
            ds18b20_read_temperature_simple();
            ds18b20_wait_for_conversion();
            printf("%d---\r\n", ds18b20_get_precission());
            ds18b20_convert_temperature_simple();
        }
#endif
}

void ds18b20_init(void) {
    one_wire_init(GPIOC, GPIO_Pin_6, TIM2);
	one_wire_list_of_devices = one_wire_search_rom(&one_wire_devices_list_size);

    if (one_wire_devices_list_size > 0) {
        temperatures = pvPortMalloc(one_wire_devices_list_size * sizeof(simple_float));
        printf("Found %d temp sensors!\r\n", one_wire_devices_list_size);
    }
    else {
        printf("No temp sensors found!\r\n");
    }

	xTaskCreate(ds18b20_task, ( char * ) "ds18b20_task", configMINIMAL_STACK_SIZE,
            NULL, tskIDLE_PRIORITY + 2, NULL);
}

u8 ds18b20_get_precission(void) {
	return precission;
}

ds18b20_devices ds18b20_get_devices(bool scan) {
    if (scan) {
        one_wire_devices_list_size = 0;
        one_wire_list_of_devices = one_wire_search_rom(&one_wire_devices_list_size);
    }
    ds18b20_devices ret;
    ret.size = one_wire_devices_list_size;
    ret.devices = one_wire_list_of_devices;
    return ret;
}

void ds18b20_set_precission(u8 p) {
	precission = p;
	one_wire_reset_pulse();

	one_wire_write_byte(0xCC); // Skip ROM
	one_wire_write_byte(0x4E); // Write scratchpad

	one_wire_write_byte(0x4B);
	one_wire_write_byte(0x46);
	// set precission
	one_wire_write_byte(0x1F | (precission << 5));
}

void ds18b20_convert_temperature_simple(void) {
	one_wire_reset_pulse();
	one_wire_write_byte(0xCC); // Skip ROM
	one_wire_write_byte(0x44); // Convert temperature
}

void ds18b20_convert_temperature(one_wire_device device) {
	one_wire_reset_pulse();
	one_wire_match_rom(device); // Match ROM
	one_wire_write_byte(0x44); // Convert temperature
}

void ds18b20_convert_temperature_all(void) {
	u8 i = 0;
	for (i = 0; i < one_wire_devices_list_size; ++i) {
		ds18b20_convert_temperature(one_wire_list_of_devices[i]);
	}
}

simple_float ds18b20_read_temperature_simple(void) {
	one_wire_reset_pulse();
	one_wire_write_byte(0xCC); // Skip ROM
	one_wire_write_byte(0xBE); // Read scratchpad

	return ds18b20_decode_temperature();
}

simple_float ds18b20_read_temperature(one_wire_device device) {
	one_wire_reset_pulse();
	one_wire_match_rom(device); // Match ROM
	one_wire_write_byte(0xBE); // Read scratchpad

	return ds18b20_decode_temperature();
}

simple_float* ds18b20_read_temperature_all(void) {
	u8 i = 0;
	if (one_wire_devices_list_size > 0) {
        for (i = 0; i < one_wire_devices_list_size; ++i) {
                temperatures[i] = ds18b20_read_temperature(one_wire_list_of_devices[i]);
        }
        return temperatures;
    }
    else {
        printf("No temp sensors\r\n");
        return NULL;
    }
}

simple_float ds18b20_decode_temperature(void) {
	int i;
	u8 crc;
	u8 data[9];
    simple_float f;
    f.is_valid = false;
	one_wire_reset_crc();

	for (i = 0; i < 9; ++i) {
		data[i] = one_wire_read_byte();
		crc = one_wire_crc(data[i]);
	}

	if (crc != 0) {
		return f;
	}

	u8 temp_msb = data[1]; // Sign byte + lsbit
	u8 temp_lsb = data[0]; // Temp data plus lsb
	float temp = (temp_msb << 8 | temp_lsb) / 16.0;
    int rest = (temp - (int)temp) * 1000.0;

    f.integer = (u8)temp;
    f.fractional = rest;
    f.is_valid = true;

    printf("%d.%d\r\n", (int)temp, rest);
    return f;
}

void ds18b20_wait_for_conversion(void) {
	if (precission == 0) {
	    vTaskDelay(95);
	} else if (precission == 1) {
	    vTaskDelay(190);
	} else if (precission == 2) {
	    vTaskDelay(380);
	} else if (precission == 3) {
	    vTaskDelay(750);
	}
}
