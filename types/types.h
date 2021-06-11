#pragma once
#include <cstdint>

#define UNUSED(x) (void)(x)

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int8  i8;
typedef int16 i16;
typedef int32 i32;
typedef int64 i64;

typedef uint8  u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;

typedef u8 byte;

typedef float f32;
typedef double f64;

#define SIZEOF(arr) (*(&arr + 1) - arr)
#define COUNTOF(arr) sizeof(arr) / sizeof(arr[0])