#ifndef NEOC_TYPES_H
#define NEOC_TYPES_H

/*
 * Modern type definitions and compiler abstractions for NeoC
 * Uses C23 features where available with fallbacks
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* C23 nullptr support */
#if __STDC_VERSION__ >= 202311L
    /* C23: nullptr is built-in */
#else
    #define nullptr ((void*)0)
#endif

/* Static assert (C11+) */
#ifndef static_assert
    #define static_assert _Static_assert
#endif

/* Attributes - use C23 syntax when available */
#if __STDC_VERSION__ >= 202311L
    #define NEOC_NODISCARD    [[nodiscard]]
    #define NEOC_MAYBE_UNUSED [[maybe_unused]]
    #define NEOC_DEPRECATED   [[deprecated]]
    #define NEOC_NORETURN     [[noreturn]]
    #define NEOC_FALLTHROUGH  [[fallthrough]]
#elif defined(__GNUC__) || defined(__clang__)
    #define NEOC_NODISCARD    __attribute__((warn_unused_result))
    #define NEOC_MAYBE_UNUSED __attribute__((unused))
    #define NEOC_DEPRECATED   __attribute__((deprecated))
    #define NEOC_NORETURN     __attribute__((noreturn))
    #define NEOC_FALLTHROUGH  __attribute__((fallthrough))
#elif defined(_MSC_VER)
    #define NEOC_NODISCARD    _Check_return_
    #define NEOC_MAYBE_UNUSED
    #define NEOC_DEPRECATED   __declspec(deprecated)
    #define NEOC_NORETURN     __declspec(noreturn)
    #define NEOC_FALLTHROUGH
#else
    #define NEOC_NODISCARD
    #define NEOC_MAYBE_UNUSED
    #define NEOC_DEPRECATED
    #define NEOC_NORETURN
    #define NEOC_FALLTHROUGH
#endif

/* Branch prediction hints */
#if defined(__GNUC__) || defined(__clang__)
    #define NEOC_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define NEOC_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define NEOC_LIKELY(x)   (x)
    #define NEOC_UNLIKELY(x) (x)
#endif

/* Inline hints */
#if defined(__GNUC__) || defined(__clang__)
    #define NEOC_INLINE       static inline __attribute__((always_inline))
    #define NEOC_NOINLINE     __attribute__((noinline))
#elif defined(_MSC_VER)
    #define NEOC_INLINE       static __forceinline
    #define NEOC_NOINLINE     __declspec(noinline)
#else
    #define NEOC_INLINE       static inline
    #define NEOC_NOINLINE
#endif

/* Alignment */
#if __STDC_VERSION__ >= 201112L
    #define NEOC_ALIGNAS(n) _Alignas(n)
    #define NEOC_ALIGNOF(t) _Alignof(t)
#elif defined(__GNUC__) || defined(__clang__)
    #define NEOC_ALIGNAS(n) __attribute__((aligned(n)))
    #define NEOC_ALIGNOF(t) __alignof__(t)
#elif defined(_MSC_VER)
    #define NEOC_ALIGNAS(n) __declspec(align(n))
    #define NEOC_ALIGNOF(t) __alignof(t)
#else
    #define NEOC_ALIGNAS(n)
    #define NEOC_ALIGNOF(t) sizeof(t)
#endif

/* Cache line alignment for performance-critical data */
#define NEOC_CACHE_LINE 64
#define NEOC_CACHE_ALIGNED NEOC_ALIGNAS(NEOC_CACHE_LINE)

/* Sized integer types with explicit width */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

/* Pointer-sized integers */
typedef uintptr_t usize;
typedef intptr_t  isize;

/* Result type for operations that can fail */
typedef enum {
    NEOC_OK = 0,
    NEOC_ERROR_ALLOC = -1,
    NEOC_ERROR_IO = -2,
    NEOC_ERROR_INVALID = -3,
    NEOC_ERROR_NOT_FOUND = -4,
    NEOC_ERROR_UNSUPPORTED = -5,
} neoc_result_t;

/* Array length macro */
#define NEOC_ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Min/max macros (type-safe in C23, fallback otherwise) */
#if __STDC_VERSION__ >= 202311L && defined(__STDC_VERSION_STDLIB_H__) && __STDC_VERSION_STDLIB_H__ >= 202311L
    /* C23 has type-generic min/max in stdlib.h */
#else
    #define NEOC_MIN(a, b) ((a) < (b) ? (a) : (b))
    #define NEOC_MAX(a, b) ((a) > (b) ? (a) : (b))
    #define NEOC_CLAMP(x, lo, hi) NEOC_MIN(NEOC_MAX(x, lo), hi)
#endif

#endif /* NEOC_TYPES_H */
