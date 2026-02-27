// Generated from cmake/config.h.in for Apple platforms (macOS / iOS).

#ifndef THIRD_PARTY_SNAPPY_OPENSOURCE_CMAKE_CONFIG_H_
#define THIRD_PARTY_SNAPPY_OPENSOURCE_CMAKE_CONFIG_H_

// Apple Clang supports all of these builtins.
#define HAVE_ATTRIBUTE_ALWAYS_INLINE 1
#define HAVE_BUILTIN_CTZ 1
#define HAVE_BUILTIN_EXPECT 1
#define HAVE_BUILTIN_PREFETCH 1

// POSIX functions available on Apple platforms.
#define HAVE_FUNC_MMAP 1
#define HAVE_FUNC_SYSCONF 1

// Third-party compression libraries (only used by snappy's own benchmarks,
// which are excluded from this build).
#define HAVE_LIBLZO2 0
#define HAVE_LIBZ 0
#define HAVE_LIBLZ4 0

// POSIX headers available on Apple platforms.
#define HAVE_SYS_MMAN_H 1
#define HAVE_SYS_RESOURCE_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_UIO_H 1
#define HAVE_UNISTD_H 1
#define HAVE_WINDOWS_H 0

// SIMD / architecture features.
// Disabled for cross-platform compatibility (x86_64 + arm64 universal builds).
#define SNAPPY_HAVE_SSSE3 0
#define SNAPPY_HAVE_X86_CRC32 0
#define SNAPPY_HAVE_BMI2 0

// NEON is available on arm64 (Apple Silicon, iOS).
#if defined(__aarch64__)
#define SNAPPY_HAVE_NEON 1
#define SNAPPY_HAVE_NEON_CRC32 1
#else
#define SNAPPY_HAVE_NEON 0
#define SNAPPY_HAVE_NEON_CRC32 0
#endif

// Apple platforms are little-endian.
#define SNAPPY_IS_BIG_ENDIAN 0

#endif  // THIRD_PARTY_SNAPPY_OPENSOURCE_CMAKE_CONFIG_H_
