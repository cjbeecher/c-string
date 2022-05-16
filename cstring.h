#ifndef C_STRING_H
#define C_STRING_H

#include <stdbool.h>
#include <stdint.h>

#define STRING_ASCII 0
#define STRING_UTF8_LE 1
#define STRING_UTF16_LE 2
#define STRING_UTF32_LE 3
#define STRING_UTF8_BE 4
#define STRING_UTF16_BE 5
#define STRING_UTF32_BE 6

struct CString {
    size_t length;
    size_t capacity;
    uint32_t *value;
};

int cstring_init(struct CString *value, size_t capacity);
int cstring_set_string(struct CString *value, unsigned char *str, size_t length, size_t increment_size, unsigned char encoding);
int cstring_append_code_point(struct CString *value, uint32_t c, size_t increment_size);
size_t cstring_to_bytes(struct CString *value, unsigned char **output, size_t max, size_t increment_size, unsigned char encoding);
void cstring_free(struct CString *value);

#endif // C_STRING_H