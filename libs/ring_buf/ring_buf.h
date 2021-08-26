#ifndef RING_BUF_H
#define RING_BUF_H

#include <stdint.h>
#include <stdbool.h>


typedef struct {
    void *buf_ptr;
    uint16_t size;
    uint8_t wlen;
    uint16_t read_index;
    uint16_t write_index;
} rbuf_t;

bool rbuf_init(rbuf_t *rbuf, void *buffer, uint16_t size, uint8_t wlen);

uint32_t rbuf_free_space(rbuf_t *rbuf);
uint32_t rbuf_full_space(rbuf_t *rbuf);
bool rbuf_isempty(rbuf_t *rbuf);
bool rbuf_isfull(rbuf_t *rbuf);
bool rbuf_flush(rbuf_t *rbuf);

bool rbuf_get(rbuf_t *rbuf, void *data);
bool rbuf_put(rbuf_t *rbuf, void *data);


#endif /* RING_BUF_H */