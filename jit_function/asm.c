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

void append_to_array(char* array, uint32_t* array_len, char* value, uint32_t value_len) {
    uint32_t old_array_len = array_len;
    array_len += value_len;
    realloc(array, (array_len) * sizeof(char));
    for (int i = 0; i < value_len; i++) {
        array[old_array_len + i] = value[i];
    }
}

/*
 * values_arr: Array with values in which the search is done
 * values_arr_len: number of elements in values_arr
 * memory: byte array uswd for returning instructions
 * x: value to search for
 * left_index: index of element left of values_arr, -1 if values_arr begins at 0 if entire search space
 */
uint32_t construct_node(uint64_t* values_arr, uint32_t values_arr_len, char** memory, uint64_t x, int64_t left_index) {
    uint32_t mid_index = values_arr_len / 2;
    
    char* tmp = NULL;
    char* l_mem = NULL;
    char* r_mem = NULL;
    
    uint32_t array_len = 0;
    
    //pseudocode below here
    //base case
    if (values_arr_len == 1) {
        append_to_array(tmp, &array_len, "CMP x < values_arr[0]");
        append_to_array(tmp, &array_len, "return left_index + 1");
        
        if (left_val == -1) {
            append_to_array(tmp, &array_len, "return NULL");
        } else {
            append_to_array(tmp, &array_len, "return left_index");
        }
        
        realloc(memory, array_len);
        memcpy(memory, tmp, array_len);
        free(tmp);
        return array_len;
    }
    
    left_array = malloc(mid_index * sizeof(char));
    for (int i = 0; i < mid_index; i++) {
        left_array[i] = values_arr[i];
    }
    
    right_array = malloc((values_arr_len - mid_index - 1) * sizeof(char));
    for (int i = mid_index + 1; i < values_arr_len; i++) {
        right_array[i] = values_arr[i];
    }
    
    //TODO: check if mid_index is the right number
    left_block = construct_node(left_array, mid_index, &l_mem, x, left_index);
    right_block = construct_node(right_array, (values_arr_len - 1) - mid_index, &r_mem, x, mid_index);
    
    //main block
    append_to_array(tmp, &array_len, "CMP if x < values_arr[mid_index]");
    append_to_array(tmp, &array_len, "jmp (len(nextline) + len(1+nextline))");
    append_to_array(tmp, &array_len, "CMP if values_arr[mid_index + 1] < x");
    append_to_array(tmp, &array_len, "return values_arr[mid_index]");
    append_to_array(tmp, &array_len, "jmp left_block  + 1"); //is len(...)
    append_to_array(tmp, &array_len, l_mem);
    append_to_array(tmp, &array_len, r_mem);
    realloc(memory, array_len);
    memcpy(memory, tmp, array_len);
    free(tmp);
    
    return array_len;
}

int main(int argc, char* argv[]) {
  if (argc < 2)
    return 1;
  uint64_t res = f(atoi(argv[1]));
  printf("res = %"SCNu64"\n", res);
  return 0;
}
