#include "global.h"

inline uint32_t atomicCompareAndSwap(volatile intptr_t *current_value,
                                int64_t old_value, int64_t new_value){
    uint32_t rc;
    __asm__ __volatile__(CAS_ASM
                        : "=a"(rc)
                        : "r"(new_value),"m"(*current_value),"0"(old_value)
                        : "cc", "memory");
    return rc;
}