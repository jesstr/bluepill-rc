#include "ring_buf.h"
#include "tinystdio.h"
#include <stdbool.h>
#include <stdint.h>


#define _size_w(n) sizeof(n) / sizeof(int)

void rbuf8_init(rbuf_t *rbuf8, uint8_t *buffer, uint32_t size) {
	rbuf8->buf_ptr = (uint32_t *)buffer;
	rbuf8->size = size;
	rbuf8->read_index = 0;
	rbuf8->write_index = 0;
}

void rbuf32_init(rbuf_t *rbuf32, uint32_t *buffer, uint32_t size) {
	rbuf32->buf_ptr = buffer;
	rbuf32->size = size;
	rbuf32->read_index = 0;
	rbuf32->write_index = 0;
}

void rbuf64_init(rbuf_t *rbuf64, uint64_t *buffer, uint32_t size) {
	rbuf64->buf_ptr = (uint32_t *)buffer;
	rbuf64->size = size;
	rbuf64->read_index = 0;
	rbuf64->write_index = 0;
}

bool rbuf_isempty(rbuf_t *rbuf) {
	if (rbuf->read_index == rbuf->write_index)
		return true;
	else
		return false;
}

/**
  * @brief  функция определения свободного места в циклическом буфере
  * (всегда остается одна свободная ячейка)
  *
  */
int32_t rbuf_free_space(rbuf_t *rbuf)
{
	int32_t free_space = 0;

	if(rbuf->write_index == rbuf->read_index)
		free_space = rbuf->size;
	else if(rbuf->write_index > rbuf->read_index)
			free_space = rbuf->size - (rbuf->write_index - rbuf->read_index);
	else
			free_space =  rbuf->read_index - rbuf->write_index;

	return free_space;
}

/**
  * @brief  функция определения заполнения циклического буфера
  * (всегда остается одна свободная ячейка)
  *
  */
int32_t rbuf_full_space(rbuf_t *rbuf)
{
	int32_t full_space = 0;

	if(rbuf->write_index == rbuf->read_index)
		full_space = 0;
	else if(rbuf->write_index > rbuf->read_index)
			full_space = rbuf->write_index - rbuf->read_index;
	else
			full_space = rbuf->size - (rbuf->read_index - rbuf->write_index);

	return full_space;
}

bool rbuf_isfull(rbuf_t *rbuf) {
	int r_tmp;
	if(rbuf_isempty(rbuf))
	{
		return false;
	}
	else
	{
		if (rbuf->write_index == (rbuf->size - 1))
		{
			r_tmp = rbuf->read_index + rbuf->size;
		}
		else
			r_tmp = rbuf->read_index;
		if (r_tmp - rbuf->write_index == 1)
			return true;
		else
			return false;
	}
}

bool rbuf8_get(rbuf_t *rbuf8, uint8_t *data) {
	/* Check whether buffer is not empty */
	if (!rbuf_isempty(rbuf8)) {
		*data = *((uint8_t*)rbuf8->buf_ptr + rbuf8->read_index);
		rbuf8->read_index++;
		/* swap the read pointer */
		if (rbuf8->read_index == (rbuf8->size))
			rbuf8->read_index = 0;
		return true;
	}
	else
		return false;
}

bool rbuf32_get(rbuf_t *rbuf32, uint32_t *data) {
	/* Check whether buffer is not empty */
	if (!rbuf_isempty(rbuf32)) {
		*data = rbuf32->buf_ptr[rbuf32->read_index++];
		/* swap the read pointer */
		if (rbuf32->read_index == (rbuf32->size))
			rbuf32->read_index = 0;
		return true;
	}
	else
		return false;
}

bool rbuf64_get(rbuf_t *rbuf64, uint64_t *data) {
	/* Check whether buffer is not empty */
	if (!rbuf_isempty(rbuf64)) {
		*data = *((uint64_t*)rbuf64->buf_ptr + rbuf64->read_index);
		rbuf64->read_index++;
		/* swap the read pointer */
		if (rbuf64->read_index == (rbuf64->size))
			rbuf64->read_index = 0;
		return true;
	}
	else 
		return false;
}


/*
 * Oldest data is overwriting when write_ptr reaches read_ptr - 1 (!).
 * For check use rbuf_isfull.
 */

bool rbuf8_put(rbuf_t *rbuf8, uint8_t data) {
	uint8_t *data_ptr;
	if(!rbuf_isfull(rbuf8))
	{
		data_ptr = (uint8_t *)rbuf8->buf_ptr + rbuf8->write_index;
		*data_ptr = data;
		rbuf8->write_index++;
		/* swap the write pointer */
		if (rbuf8->write_index == rbuf8->size)
			rbuf8->write_index = 0;
		return true;
	}
	else
		return false;
}

bool rbuf32_put(rbuf_t *rbuf32, uint32_t data) {
	if(!rbuf_isfull(rbuf32))
	{
		rbuf32->buf_ptr[rbuf32->write_index++] = data;
		/* swap the write pointer */
		if (rbuf32->write_index == (rbuf32->size))
			rbuf32->write_index = 0;
		return true;
	}
	else
		return false;
}

bool rbuf64_put(rbuf_t *rbuf64, uint64_t data) {
	uint32_t *data_ptr;
	char size_w  = _size_w(data);
	if(!rbuf_isfull(rbuf64))
	{
		data_ptr = rbuf64->buf_ptr + rbuf64->write_index * size_w;
		*(uint64_t*)data_ptr = data;
		rbuf64->write_index++;
		/* swap the write pointer */
		if (rbuf64->write_index == rbuf64->size)
			rbuf64->write_index = 0;
		return true;
	}
	else
		return false;
}

/**
  * @brief  тестовая функция для циклического буфера
  *
  */
bool buf_test(void) {
	rbuf_t rbuf32;
	rbuf_t rbuf64;
	uint32_t rbuf_data32[32];
	uint64_t rbuf_data64[32];
	unsigned int i;
	rbuf32_init(&rbuf32, rbuf_data32, 32);
	rbuf64_init(&rbuf64, rbuf_data64, 32);
	bool status_test = true;

	uint32_t put32 = 0, get32 = 0;
	uint64_t put64 = 0x5555555555555555, get64 = 0;
	int j;

	DBG printf("capacity 64 = %u\r\n", (unsigned int)rbuf64.size);

	DBG printf("RBUF64 TEST:\r\n");
	DBG printf("FULLNESS TEST:\r\n");
	i = 0;
	while (rbuf64_put(&rbuf64, put64++)) {
		DBG printf("W%u: %.16llX \r\n", i, (long long)rbuf_data64[i]);
		i ++;
		j = 10000;
		while (j--);
	}
	if(i != rbuf64.size)
		status_test = false;

	DBG printf("EMPTYNESS TEST:\r\n");
    i = 0;
	while (rbuf64_get(&rbuf64, &get64)) {
		if(get64 != rbuf_data64[i])
			status_test = false;

		DBG printf("R%u: %.16llX \r\n", i++, (long long)get64);
		j = 10000;
		while (j--);
	}
	if(i != rbuf64.size)
		status_test = false;

	i = 0;
	put64 = 0x4444444444444444;
	rbuf64_init(&rbuf64, rbuf_data64, 32);
	DBG printf("CYCLE TEST:\r\n");
	for(i = 0; i < 2*rbuf64.size; i++)
	{
		if(rbuf64_put(&rbuf64, put64++)) {
			DBG printf("W%u: %.16llX \r\n", i, (long long)rbuf_data64[i-32*(i/32)]);
		} else {
			status_test = false;
		}
		j = 10000;
		while (j--);
		if(rbuf64_get(&rbuf64, &get64)) {
			if(get64 != rbuf_data64[i-32*(i/32)]) {
				status_test = false;
			}
			DBG printf("R%u: %.16llX \r\n", i, (long long)get64);
		} else {
			status_test = false;
		}
		j = 10000;
		while (j--);

	}

	DBG printf("capacity 32 = %u\r\n", (unsigned int)rbuf32.size);

	DBG printf("RBUF32 TEST:\r\n");
	DBG printf("FULLNESS TEST:\r\n");
	i = 0;
	while (rbuf32_put(&rbuf32, put32++)) {
		DBG printf("W%u: %.16lX \r\n", i, rbuf_data32[i]);
		i ++;
		j = 10000;
		while (j--);
	}
	if(i != rbuf32.size)
		status_test = false;

	DBG printf("EMPTYNESS TEST:\r\n");
    i = 0;
	while (rbuf32_get(&rbuf32, &get32)) {
		if(get32 != rbuf_data32[i]) {
			status_test = false;
		}
		DBG printf("R%u: %.16lX \r\n", i++, get32);
		j = 10000;
		while (j--);
	}
	if(i != rbuf32.size) {
		status_test = false;
	}

	i = 0;
	put32 = 0x44444444;
	rbuf32_init(&rbuf32, rbuf_data32, 32);
	DBG printf("CYCLE TEST:\r\n");
	for(i = 0; i < 2*rbuf64.size; i++) {
		if(rbuf32_put(&rbuf32, put32++)) {
			DBG printf("W%d: %.16lX \r\n", i, rbuf_data32[i-32*(i/32)]);
		} else {
			status_test = false;
		}
		j = 10000;
		while (j--);
		if(rbuf32_get(&rbuf32, &get32))	{
			if(get32 != rbuf_data32[i-32*(i/32)]) {
				status_test = false;
			}
			DBG printf("R%u: %.16lX \r\n", i, get32);
		} else {
			status_test = false;
		}
		j = 10000;
		while (j--);
	}
	return status_test;
}		