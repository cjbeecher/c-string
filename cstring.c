#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "cstring.h"

#define _STRING_ENCODED_SIZE 16

uint32_t _fetch_32_le(unsigned char **data, size_t *length) {
    int32_t value;
    value = (*data)[0];
    value |= (*data)[1];
    value |= (*data)[2];
    value |= (*data)[3];
    *data += 4;
    *length -= 4;
    return value;
}

size_t _encode_32_le(uint32_t value, unsigned char **output) {
    size_t written = 4;
    (*output)[0] = value & 0xFF;
    (*output)[1] = (value >> 8) & 0xFF;
    (*output)[2] = (value >> 16) & 0xFF;
    (*output)[3] = (value >> 24) & 0xFF;
    return written;
}

uint32_t _fetch_32_be(unsigned char **data, size_t *length) {
    int32_t value;
    value = (*data)[3];
    value |= (*data)[2];
    value |= (*data)[1];
    value |= (*data)[0];
    *data += 4;
    *length -= 4;
    return value;
}

size_t _encode_32_be(uint32_t value, unsigned char **output) {
    size_t written = 4;
    (*output)[0] = (value >> 24) & 0xFF;
    (*output)[1] = (value >> 16) & 0xFF;
    (*output)[2] = (value >> 8) & 0xFF;
    (*output)[3] = value & 0xFF;
    return written;
}

uint32_t _fetch_16_le(unsigned char **data, size_t *length) {
    int32_t tmp;
    int32_t value;
    value = (*data)[0];
    value |= (*data)[1];
    if (value > 0xD7FF && value < 0xE000) {
        tmp = (*data)[2];
        tmp |= (*data)[3];
        value = (value - 0xD800) * 0x400;
        tmp = tmp -  0xDC00;
        value += tmp + 0x10000;
        *data += 2;
        *length -= 2;
    }
    *data += 2;
    *length -= 2;
    return value;
}

size_t _encode_16_le(uint32_t value, unsigned char **output) {
    uint32_t tmp;
    size_t written = 0;
    if (value >= 0x010000 && value <= 0x10FFFF) {
        written = 4;
        value = value - 0x10000;
        tmp = (value >> 10) + 0xD800;
        (*output)[0] = tmp & 0xFF;
        (*output)[1] = (tmp >> 8) & 0xFF;
        tmp = (value % 0x400) +  0xDC00;
        (*output)[2] = tmp & 0xFF;
        (*output)[3] = (tmp >> 8) & 0xFF;
    }
    else {
        written = 2;
        (*output)[0] = value & 0xFF;
        (*output)[1] = (value >> 8) & 0xFF;
    }
    return written;
}

uint32_t _fetch_16_be(unsigned char **data, size_t *length) {
    int32_t tmp;
    int32_t value;
    value = (*data)[1];
    value |= (*data)[0];
    if (value > 0xD7FF && value < 0xE000) {
        tmp = (*data)[3];
        tmp |= (*data)[2];
        value = (value - 0xD800) * 0x400;
        tmp = tmp -  0xDC00;
        value += tmp + 0x10000;
        *data += 2;
        *length -= 2;
    }
    *data += 2;
    *length -= 2;
    return value;
}

size_t _encode_16_be(uint32_t value, unsigned char **output) {
    uint32_t tmp;
    size_t written = 0;
    if (value >= 0x010000 && value <= 0x10FFFF) {
        written = 4;
        value = value - 0x10000;
        tmp = (value >> 10) + 0xD800;
        (*output)[0] = (tmp >> 8) & 0xFF;
        (*output)[1] = tmp & 0xFF;
        tmp = (value % 0x400) +  0xDC00;
        (*output)[2] = (tmp >> 8) & 0xFF;
        (*output)[3] = tmp & 0xFF;
    }
    else {
        written = 2;
        (*output)[0] = (value >> 8) & 0xFF;
        (*output)[1] = value & 0xFF;
    }
    return written;
}

uint32_t _fetch_8_be(unsigned char **data, size_t *length) {
    int32_t value;
    value = (*data)[0];
    if (value >> 7 == 0) {
        *data += 1;
        *length -= 1;
        return value;
    }
    if (value >> 5 == 0x06) {
        value = ((value & 0x1F) << 6) | ((*data)[1] & 0x3F);
        *data += 2;
        *length -= 2;
        return value;
    }
    if (value >> 4 == 0x0E) {
        value = ((value & 0x0F) << 6) | ((*data)[1] & 0x3F);
        value = (value << 6) | ((*data)[2] & 0x3F);
        *data += 3;
        *length -= 3;
        return value;
    }
    if (value >> 3 == 0x1E) {
        value = ((value & 0x07) << 6) | ((*data)[1] & 0x3F);
        value = (value << 6) | ((*data)[2] & 0x3F);
        value = (value << 6) | ((*data)[3] & 0x3F);
        *data += 4;
        *length -= 4;
        return value;
    }
    return 0xFEFF;
}
size_t _encode_8_be(uint32_t value, unsigned char **output) {
    size_t written = 0;
    if (value & 0x7F == value) {
        written = 1;
        (*output)[0] = value & 0xFF;
    }
    else if (value & 0x7FF == value) {
        written = 2;
        (*output)[0] = ((value >> 8) & 0x1F) | 0xC0;
        (*output)[1] = (value & 0x3F) | 0x80;
    }
    else if (value & 0xFFFF == value) {
        written = 3;
        (*output)[0] = ((value >> 16) & 0x0F) | 0xE0;
        (*output)[1] = ((value >> 8) & 0x3F) | 0x80;
        (*output)[2] = (value & 0x3F) | 0x80;
    }
    else if (value & 0x1FFFFF == value) {
        written = 4;
        (*output)[0] = ((value >> 24) & 0x07) | 0xF0;
        (*output)[1] = ((value >> 16) & 0x3F) | 0x80;
        (*output)[2] = ((value >> 8) & 0x3F) | 0x80;
        (*output)[3] = (value & 0x3F) | 0x80;
    }
    return written;
}

int cstring_init(struct CString *value, size_t capacity) {
    value->value = malloc(capacity * sizeof(uint32_t));
    if (value->value == NULL) return 0;
    value->length = 0;
    value->capacity = capacity;
    return 1;
}

int cstring_set_string(struct CString *value, unsigned char *str, size_t length, size_t increment_size, unsigned char encoding) {
    size_t index;
    uint32_t char_value;
    uint32_t *tmp_uint32;
    unsigned char *tmp;
    if (length + 1 > value->capacity) {
        index = (length + 1) - ((length + 1) % increment_size);
        tmp_uint32 = realloc(value->value, (length + 1) * sizeof(uint32_t));
        if (tmp_uint32 == NULL) return 0;
        value->value = tmp_uint32;
        value->capacity = length + 1;
    }
    value->length = 0;
    tmp = str;
    switch (encoding) {
        case STRING_ASCII:
            for (index = 0; index < length; index++)
                value->value[index] = str[index];
            break;
        case STRING_UTF8_LE:
        case STRING_UTF8_BE:
            while (length > 0) {
                value->value[value->length] = _fetch_8_be(&tmp, &length);
                if (value->value[value->length] == 0xFEFF) {
                    if (value->length == 0) continue;
                    else return 0;
                }
                value->length++;
            }
            value->value[value->length] = 0;
            break;
        case STRING_UTF16_LE:
            while (length > 0) {
                value->value[value->length] = _fetch_16_le(&tmp, &length);
                if (value->value[value->length] == 0xFEFF) {
                    if (value->length == 0) continue;
                    else return 0;
                }
                value->length++;
            }
            value->value[value->length] = 0;
            break;
        case STRING_UTF32_LE:
            while (length > 0) {
                value->value[value->length] = _fetch_32_le(&tmp, &length);
                if (value->value[value->length] == 0xFEFF) {
                    if (value->length == 0) continue;
                    else return 0;
                }
                value->length++;
            }
            value->value[value->length] = 0;
            break;
        case STRING_UTF16_BE:
            while (length > 0) {
                value->value[value->length] = _fetch_16_be(&tmp, &length);
                if (value->value[value->length] == 0xFEFF) {
                    if (value->length == 0) continue;
                    else return 0;
                }
                value->length++;
            }
            value->value[value->length] = 0;
            break;
        case STRING_UTF32_BE:
            while (length > 0) {
                value->value[value->length] = _fetch_32_be(&tmp, &length);
                if (value->value[value->length] == 0xFEFF) {
                    if (value->length == 0) continue;
                    else return 0;
                }
                value->length++;
            }
            value->value[value->length] = 0;
            break;
        default:
            return 1;
    }
    return 1;
}

int cstring_append_code_point(struct CString *value, uint32_t c, size_t increment_size) {
    uint32_t *tmp;
    if (value->capacity == value->length + 1) {
        tmp = realloc(value->value, (value->capacity + increment_size) * sizeof(uint32_t));
        if (tmp == NULL) return 0;
        value->value = tmp;
    }
    value->value[value->length] = c;
    value->length++;
    value->value[value->length] = 0;
    return 1;
}

size_t cstring_to_bytes(struct CString *value, unsigned char **output, size_t max, size_t increment_size, unsigned char encoding) {
    size_t index;
    size_t tmp = 0;
    size_t written = 0;
    size_t tmp_realloc = 0;
    unsigned char *tmp_char;
    unsigned char *char_ptr;
    unsigned char encoded_char[_STRING_ENCODED_SIZE];
    if (increment_size < _STRING_ENCODED_SIZE)
        increment_size = (((_STRING_ENCODED_SIZE - (_STRING_ENCODED_SIZE % increment_size)) / increment_size) + 1) * increment_size;
    switch (encoding) {
        case STRING_ASCII:
            if (value->length + 1 > max) {
                tmp = (value->length - (value->length % increment_size)) + increment_size;
                tmp_char = realloc(*output, max + increment_size + 1);
                if (tmp_char == NULL) return 0;
                max += increment_size + 1;
                *output = tmp_char;
            }
            for (tmp = 0; tmp < value->length; tmp++)
                (*output)[tmp] = value->value[tmp] && 0xFF;
            (*output)[value->length] = '\0';
            break;
        case STRING_UTF8_LE:
        case STRING_UTF8_BE:
            char_ptr = (unsigned char *)encoded_char;
            for (index = 0; index < value->length; index++) {
                tmp = _encode_8_be(value->value[index], &char_ptr);
                if (tmp + written > max) {
                    tmp_char = realloc(*output, max + increment_size);
                    if (tmp_char == NULL) return 0;
                    max += increment_size;
                    *output = tmp_char;
                }
                memcpy(&((*output)[written]), encoded_char, tmp);
                written += tmp;
            }
            break;
        case STRING_UTF16_LE:
            char_ptr = (unsigned char *)encoded_char;
            (*output)[0] = 0xFF;
            (*output)[1] = 0xFE;
            written = 2;
            for (index = 0; index < value->length; index++) {
                tmp = _encode_16_le(value->value[index], &char_ptr);
                if (tmp + written > max) {
                    tmp_char = realloc(*output, max + increment_size);
                    if (tmp_char == NULL) return 0;
                    max += increment_size;
                    *output = tmp_char;
                }
                memcpy(&((*output)[written]), encoded_char, tmp);
                written += tmp;
            }
            break;
        case STRING_UTF32_LE:
            char_ptr = (unsigned char *)encoded_char;
            (*output)[0] = 0xFF;
            (*output)[1] = 0xFE;
            (*output)[2] = 0x0;
            (*output)[3] = 0x0;
            written = 4;
            for (index = 0; index < value->length; index++) {
                tmp = _encode_32_le(value->value[index], &char_ptr);
                if (tmp + written > max) {
                    tmp_char = realloc(*output, max + increment_size);
                    if (tmp_char == NULL) return 0;
                    max += increment_size;
                    *output = tmp_char;
                }
                memcpy(&((*output)[written]), encoded_char, tmp);
                written += tmp;
            }
            break;
        case STRING_UTF16_BE:
            (*output)[0] = 0xFE;
            (*output)[1] = 0xFF;
            written = 2;
            char_ptr = (unsigned char *)encoded_char;
            for (index = 0; index < value->length; index++) {
                tmp = _encode_16_be(value->value[index], &char_ptr);
                if (tmp + written > max) {
                    tmp_char = realloc(*output, max + increment_size);
                    if (tmp_char == NULL) return 0;
                    max += increment_size;
                    *output = tmp_char;
                }
                memcpy(&((*output)[written]), encoded_char, tmp);
                written += tmp;
            }
            break;
        case STRING_UTF32_BE:
            (*output)[0] = 0x0;
            (*output)[1] = 0x0;
            (*output)[2] = 0xFE;
            (*output)[3] = 0xFF;
            written = 4;
            char_ptr = (unsigned char *)encoded_char;
            for (index = 0; index < value->length; index++) {
                tmp = _encode_32_be(value->value[index], &char_ptr);
                if (tmp + written > max) {
                    tmp_char = realloc(*output, max + increment_size);
                    if (tmp_char == NULL) return 0;
                    max += increment_size;
                    *output = tmp_char;
                }
                memcpy(&((*output)[written]), encoded_char, tmp);
                written += tmp;
            }
            break;
        default:
            return 0;
    }
    return written;
}

void cstring_free(struct CString *value) {
    free(value->value);
    value->length = -1;
    value->capacity = -1;
    value->value = NULL;
}