#ifndef ASM_H
#define ASM_H 1

#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <string.h>

uint32_t construct_node(uint64_t* values_arr, uint32_t values_arr_length, uint32_t low_index, uint32_t hi_index, char** memory);

#endif //ASM_H
