#include <stdio.h>
#include <stdlib.h>
#include "cstring.h"


#define _INCREMENT_SIZE 4
#define _STRING_MAX 8

int main() {
    // a = 0x61
    size_t written;
    struct CString string;
    if (cstring_init(&string, _INCREMENT_SIZE) != 1) {
        printf("Error initializing string\n");
        return 1;
    }
    if (cstring_set_string(&string, "aaa", 3, _INCREMENT_SIZE, STRING_UTF8_BE) != 1) {
        printf("Error setting string\n");
        cstring_free(&string);
        return 1;
    }
    if (string.value[0] != 'a' || string.value[1] != 'a' || string.value[2] != 'a' || string.value[3] != 0) {
        printf("Error setting string\n");
        cstring_free(&string);
        return 1;
    }
    if (cstring_set_string(&string, "a\0a\0a\0", 6, _INCREMENT_SIZE, STRING_UTF16_LE) != 1) {
        printf("Error setting string\n");
        cstring_free(&string);
        return 1;
    }
    if (string.value[0] != 'a' || string.value[1] != 'a' || string.value[2] != 'a' || string.value[3] != 0) {
        printf("Error setting string\n");
        cstring_free(&string);
        return 1;
    }
    string.value[0] = 0x10437;
    unsigned char *output = malloc(_STRING_MAX * sizeof(unsigned char));
    written = cstring_to_bytes(&string, &output, _STRING_MAX, _INCREMENT_SIZE, STRING_UTF16_LE);
    if (written == 0) {
        printf("Error writing to bytes/output as utf16 le\n");
        cstring_free(&string);
        free(output);
        return 1;
    }
    if (output[0] != 0xFF || output[1] != 0xFE || output[2] != 0x1 || output[3] != 0xD8 || output[4] != 0x37 || output[5] != 0xDC) {
        printf("String does not match output for utf16 le\n");
    }
    if (written != 10) {
        printf("Written does not match for utf16 le: %li\n", written);
    }
    written = cstring_to_bytes(&string, &output, _STRING_MAX, _INCREMENT_SIZE, STRING_UTF16_BE);
    if (written == 0) {
        printf("Error writing to bytes/output as utf16 be\n");
        cstring_free(&string);
        free(output);
        return 1;
    }
    if (output[1] != 0xFF || output[0] != 0xFE || output[3] != 0x1 || output[2] != 0xD8 || output[5] != 0x37 || output[4] != 0xDC) {
        printf("String does not match output for utf16 be\n");
    }
    if (written != 10) {
        printf("Written does not match for utf16 be: %li\n", written);
    }
    written = cstring_to_bytes(&string, &output, _STRING_MAX, _INCREMENT_SIZE, STRING_UTF32_LE);
    if (written == 0) {
        printf("Error writing to bytes/output as utf32 le\n");
        cstring_free(&string);
        free(output);
        return 1;
    }
    if (output[0] != 0xFF || output[1] != 0x0FE || output[2] != 0x0 || output[3] != 0x0 || output[4] != 0x37 || output[5] != 0x04 || output[6] != 0x01 || output[7] != 0x00) {
        printf("String does not match output for utf32 le\n");
    }
    if (written != 16) {
        printf("Written does not match for utf32 le: %li\n", written);
    }
    written = cstring_to_bytes(&string, &output, _STRING_MAX, 3, STRING_UTF32_BE);
    if (written == 0) {
        printf("Error writing to bytes/output as utf32 be\n");
        cstring_free(&string);
        free(output);
        return 1;
    }
    if (output[3] != 0xFF || output[2] != 0x0FE || output[1] != 0x0 || output[0] != 0x0 || output[7] != 0x37 || output[6] != 0x04 || output[5] != 0x01 || output[4] != 0x00) {
        printf("String does not match output for utf32 be\n");
    }
    if (written != 16) {
        printf("Written does not match for utf32 be: %li\n", written);
    }
    free(output);
    cstring_free(&string);
    return 0;
}
