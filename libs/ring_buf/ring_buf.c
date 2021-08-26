#include <stdint.h>
#include <stdbool.h>
#include "ring_buf.h"


bool rbuf_init(rbuf_t *rbuf, void *buffer, uint16_t size, uint8_t wlen)
{
    if (!rbuf || !buffer || !size || !wlen ||
        wlen > 8 || ((wlen % 2) && wlen > 1)) {
        return false;
    }

    rbuf->buf_ptr = buffer;
    rbuf->size = size;
    rbuf->wlen = wlen;
    rbuf->read_index = 0;
    rbuf->write_index = 0;

    return true;
}


bool rbuf_flush(rbuf_t *rbuf)
{
    if (!rbuf) {
        return false;
    }

    rbuf->read_index = 0;
    rbuf->write_index = 0;

    return true;
}


bool rbuf_isempty(rbuf_t *rbuf)
{
    if (!rbuf) return false;

    if (rbuf->read_index == rbuf->write_index) {
        return true;
    } else {
        return false;
    }
}

/* Get number of free available cells. One cell is always stay reserved */
uint32_t rbuf_free_space(rbuf_t *rbuf)
{
    if (!rbuf) return 0;

    uint32_t free_space = 0;

    if (rbuf->write_index == rbuf->read_index) {
        free_space = rbuf->size;
    } else if (rbuf->write_index > rbuf->read_index) {
        free_space = rbuf->size - (rbuf->write_index - rbuf->read_index);
    } else {
        free_space = rbuf->read_index - rbuf->write_index;
    }

    /* One cell is always reserved to detect rbuf fullness */
    if (free_space) free_space--;

    return free_space;
}

/* Get number of used cells (number of stored items). One cell is always stay reserved */
uint32_t rbuf_full_space(rbuf_t *rbuf)
{
    if (!rbuf) return 0;

    uint32_t full_space = 0;

    if (rbuf->write_index == rbuf->read_index) {
        full_space = 0;
    } else if (rbuf->write_index > rbuf->read_index) {
        full_space = rbuf->write_index - rbuf->read_index;
    } else {
        full_space = rbuf->size - (rbuf->read_index - rbuf->write_index);
    }

    return full_space;
}


bool rbuf_isfull(rbuf_t *rbuf)
{
    if (!rbuf) return false;

    uint16_t r_tmp;
    if (rbuf->write_index == (rbuf->size - 1)) {
        r_tmp = rbuf->read_index + rbuf->size;
    } else {
        r_tmp = rbuf->read_index;
    }
    if (r_tmp - rbuf->write_index == 1) {
        return true;
    } else {
        return false;
    }
}


bool rbuf_get(rbuf_t *rbuf, void *data)
{
    if (!rbuf || !data || rbuf_isempty(rbuf)) {
        return false;
    }

    switch (rbuf->wlen) {
        case 1:
            *(uint8_t *)data = *((uint8_t *)rbuf->buf_ptr + rbuf->read_index++);
            break;
        case 2:
            *(uint16_t *)data = *((uint16_t *)rbuf->buf_ptr + rbuf->read_index++);
            break;
        case 4:
            *(uint32_t *)data = *((uint32_t *)rbuf->buf_ptr + rbuf->read_index++);
            break;
        case 8:
            *(uint64_t *)data = *((uint64_t *)rbuf->buf_ptr + rbuf->read_index++);
            break;
        default:
            return false;
    }
    if (rbuf->read_index == (rbuf->size)) {
        rbuf->read_index = 0;
    }

    return true;
}


bool rbuf_put(rbuf_t *rbuf, void *data)
{
    if (!rbuf || !data || rbuf_isfull(rbuf)) {
        return false;
    }
    void *data_ptr = rbuf->buf_ptr + rbuf->write_index * rbuf->wlen;
    switch (rbuf->wlen) {
        case 1:
            *(uint8_t *)data_ptr = *(uint8_t *)data;
            break;
        case 2:
            *(uint16_t *)data_ptr = *(uint16_t *)data;
            break;
        case 4:
            *(uint32_t *)data_ptr = *(uint32_t *)data;
            break;
        case 8:
            *(uint64_t *)data_ptr = *(uint64_t *)data;
            break;
        default:
            return false;
    }
    rbuf->write_index++;
    if (rbuf->write_index == rbuf->size) {
        rbuf->write_index = 0;
    }

    return true;
}