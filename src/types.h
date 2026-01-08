#ifndef TYPES_H
#define TYPES_H

// Minimal fixed-width integer types for freestanding build
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long long uint64_t;

typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed int     int32_t;
typedef signed long long int64_t;

// Size type
typedef unsigned long size_t;

// Minimal C library declarations
extern "C" {
    void* memcpy(void* dst, const void* src, size_t n);
    void* memset(void* p, int c, size_t n);
    int strcmp(const char* a, const char* b);
    char* strncpy(char* dst, const char* src, size_t n);
    size_t strlen(const char* s);
}

#endif // TYPES_H