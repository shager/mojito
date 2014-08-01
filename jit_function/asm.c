#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

uint64_t f(uint64_t x) {
  if (x < 1000000000)
    return 10;
  if (x < 0xffffffffffff)
    return 20;
  if (x < 16)
    return 30;
  return 100;
}

uint32_t construct_node(uint64_t* values_arr, uint32_t values_arr_len, uint32_t* memory, uint32_t* memory_len, uint64_t x) {
    uint32_t mid_index = values_arr_len / 2;
    
    //pseudocode below here
    
    uint32_t l_mem_len = 0;
    uint32_t r_mem_len = 0;
    
    left_block = construct_node(values_arr[0:mid_index], mid_index - 1, l_mem, &l_mem_len, x)
    right_block = construct_node(values_arr[mid_index + 1:values_arr_len], mid_index - 1, r_mem, &r_mem_len, x)
    
    //main block
    uint64_t tmp = CMP if x < values_arr[mid_index]
    tmp += jmp (len(nextline) + len(1+nextline))
    tmp += CMP if values_arr[mid_index + 1] < x
    tmp += return values_arr[mid_index]
    tmp += jmp left_block  + 1 //is len(...)
    tmp += l_mem
    tmp += r_mem
    *memory = tmp;
    //EOmain block
    
    return left_block + right_block + len(main block)
}

int main(int argc, char* argv[]) {
  if (argc < 2)
    return 1;
  uint64_t res = f(atoi(argv[1]));
  printf("res = %"SCNu64"\n", res);
  return 0;
}
