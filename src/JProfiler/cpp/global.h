#include <stdint.h>

#define CAS_ASM "lock; cmpxchgl %1 %2"
#define MAX_STRING_BUF 2000
#define MAX_FRAMES 128
#define MAX_FILENAME 500

inline uint32_t atomicCompareAndSwap(volatile intptr_t *current_value,
                                int64_t old_value, int64_t new_value);