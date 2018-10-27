#ifndef RING_BUF_H
#define RING_BUF_H

#include "common.h"
#include <stdbool.h>
#include <stdint.h>


typedef struct {
	uint32_t *buf_ptr;
	uint32_t size;
	uint32_t read_index;
	uint32_t write_index;
} rbuf_t;

void rbuf8_init(rbuf_t *rbuf8, uint8_t *buffer, uint32_t size);
void rbuf32_init(rbuf_t *rbuf32, uint32_t *buffer, uint32_t size);
void rbuf64_init(rbuf_t *rbuf64, uint64_t *buffer, uint32_t size);
bool rbuf_isempty(rbuf_t *rbuf);
int32_t rbuf_free_space(rbuf_t *rbuf);
int32_t rbuf_full_space(rbuf_t *rbuf);
bool rbuf_isfull(rbuf_t *rbuf);
bool rbuf8_get(rbuf_t *rbuf8, uint8_t *data);
bool rbuf32_get(rbuf_t *rbuf32, uint32_t *data);
bool rbuf64_get(rbuf_t *rbuf64, uint64_t *data);
bool rbuf8_put(rbuf_t *rbuf8, uint8_t data);
bool rbuf32_put(rbuf_t *rbuf32, uint32_t data);
bool rbuf64_put(rbuf_t *rbuf64, uint64_t data);
bool buf_test(void);

#endif