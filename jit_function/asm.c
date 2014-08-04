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

void append_to_array(char** array, uint32_t* array_len, char* value, uint32_t value_len) {
    uint32_t old_array_len = *array_len;
    *array_len += value_len;
    *array = (char*)realloc(*array, (*array_len) * sizeof(char));
    for (int i = 0; i < value_len; i++) {
        (*array)[old_array_len + i] = value[i];
    }
}

//push %rbp
void push(char** mem, uint32_t* mem_len) {
    char* opcode_buffer = malloc(1 * sizeof(char));
    opcode_buffer[0] = 0x55;
    append_to_array(mem, mem_len, opcode_buffer, 1);
    free(opcode_buffer);
}

//mov target to eax
void mov(char** mem, uint32_t* mem_len, int32_t target) {
    char* opcode_buffer = malloc(5 * sizeof(char));
    opcode_buffer[0] = 0xb8; //mov $target,%eax
    //next 4 bytes are the number target (in little endian)
    opcode_buffer[1] = (char)(target % 0xFF);
    opcode_buffer[2] = (char)((target >> 8) % 0xFF);
    opcode_buffer[3] = (char)((target >> 16) % 0xFF);
    opcode_buffer[4] = (char)((target >> 24) % 0xFF);
    append_to_array(mem, mem_len, opcode_buffer, 5);
    free(opcode_buffer);
}

//move stack pointer to base pointer, arg1 to rbp-8
void mov_stack(char** mem, uint32_t* mem_len) {
    char* opcode_buffer = malloc(7 * sizeof(char));
    opcode_buffer[0] = 0x48; //mov %rsp,%rbp
    opcode_buffer[1] = 0x89;
    opcode_buffer[2] = 0xe5;
    opcode_buffer[3] = 0x48; //mov %rdi,-0x8(%rbp)
    opcode_buffer[4] = 0x89;
    opcode_buffer[5] = 0x7d;
    opcode_buffer[6] = 0xf8;
    append_to_array(mem, mem_len, opcode_buffer, 7);
    free(opcode_buffer);
}

//pop rbp and return
void pop_ret(char** mem, uint32_t* mem_len) {
    char* opcode_buffer = malloc(2 * sizeof(char));
    opcode_buffer[0] = 0x5d; //pop %rbp
    opcode_buffer[1] = 0xc3; //retq
    append_to_array(mem, mem_len, opcode_buffer, 2);
    free(opcode_buffer);
}

//cmp quadword (smaller than 64 bit)
void cmpq(char** mem, uint32_t* mem_len, uint64_t x) {
    char* opcode_buffer = malloc(8 * sizeof(char));
    opcode_buffer[0] = 0x48;
    opcode_buffer[1] = 0x81;
    opcode_buffer[2] = 0x7d;
    opcode_buffer[3] = 0xf8;
    //next 4 bytes are the number x (in little endian)
    opcode_buffer[4] = (char)(x % 0xFF);
    opcode_buffer[5] = (char)((x >> 8) % 0xFF);
    opcode_buffer[6] = (char)((x >> 16) % 0xFF);
    opcode_buffer[7] = (char)((x >> 24) % 0xFF);
    append_to_array(mem, mem_len, opcode_buffer, 8);
    free(opcode_buffer);
}

//cmp quadword (32 - 64 bit)
void mov_and_cmpq(char** mem, uint32_t* mem_len, uint64_t x) {
    char* opcode_buffer = malloc(14 * sizeof(char));
    opcode_buffer[0] = 0x48; //movabs x,%rax
    opcode_buffer[1] = 0xb8;
    //next 8 bytes are the number x (in little endian)
    opcode_buffer[2] = (char)(x % 0xFF);
    opcode_buffer[3] = (char)((x >> 8) % 0xFF);
    opcode_buffer[4] = (char)((x >> 16) % 0xFF);
    opcode_buffer[5] = (char)((x >> 24) % 0xFF);
    opcode_buffer[6] = (char)((x >> 32) % 0xFF);
    opcode_buffer[7] = (char)((x >> 40) % 0xFF);
    opcode_buffer[8] = (char)((x >> 48) % 0xFF);
    opcode_buffer[9] = (char)((x >> 56) % 0xFF);
    opcode_buffer[10] = 0x48; //cmp %rax,-0x8(%rbp)
    opcode_buffer[11] = 0x39;
    opcode_buffer[12] = 0x45;
    opcode_buffer[13] = 0xf8;
    append_to_array(mem, mem_len, opcode_buffer, 14);
    free(opcode_buffer);
}

//Jump below or equal
void jbe(char** mem, uint32_t* mem_len, char offset) {
    char* opcode_buffer = malloc(2 * sizeof(char));
    opcode_buffer[0] = 0x76; //jbe by offset bytes
    opcode_buffer[1] = offset;
    append_to_array(mem, mem_len, opcode_buffer, 2);
    free(opcode_buffer);
}

uint32_t safe_cmpq(char** mem, uint32_t* mem_len, uint64_t x) {
    if ((x & 0xFFFFFFFF) == x) {
        cmpq(&tmp, &array_len, x);
        return 8;
    } else {
        mov_and_cmpq(&tmp, &array_len, x);
        return 14;
    }
}

/*
 * values_arr: Array with values in which the search is done
 * values_arr_len: number of elements in values_arr
 * memory: byte array uswd for returning instructions
 * x: value to search for
 * left_index: index of element left of values_arr, -1 if values_arr begins at 0 if entire search space
 */
uint32_t construct_node(uint64_t* values_arr, uint32_t values_arr_len, char** memory, int64_t left_index) {
    uint32_t mid_index = values_arr_len / 2;
    
    char* tmp = NULL;
    char* l_mem = NULL;
    char* r_mem = NULL;
    
    uint32_t array_len = 0;
    
    //function entering, at first iteration only __TODO__
    push(&tmp, &array_len);
    mov_stack(&tmp, &array_len);

    //base case
    if (values_arr_len == 1) {
        safe_cmpq(&tmp, &array_len, values_arr[0]);
        //TODO: check if ja or jbe
        jbe(&tmp, &array_len, 0x7);
        
        //return left_index + 1
        mov(&tmp, &array_len, (uint32_t)left_index + 1);
        pop_ret(&tmp, &array_len);
        
        if (left_val == -1) {
            //return NULL
            mov(&tmp, &array_len, 0x0);
            pop_ret(&tmp, &array_len);
        } else {
            //return left index
            mov(&tmp, &array_len, (uint32_t)left_index);
            pop_ret(&tmp, &array_len);
        }
        
        *memory = tmp;
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
    
    left_block = construct_node(left_array, mid_index, l_mem, left_index);
    right_block = construct_node(right_array, (values_arr_len - 1) - mid_index, r_mem, mid_index);
    
    //main block
    // cmpq x < values_arr[mid_index]
    safe_cmpq(&tmp, &array_len, values_arr[mid_index]);
    jbe(&tmp, &array_len, ); //TODO: offset (jmp (len(nextline) + len(1+nextline)))
    // cmpq values_arr[mid_index + 1] < x
    safe_cmpq(&tmp, &array_len, values_arr[mid_index + 1]);
    jbe(&tmp, &array_len, ); //TODO: offset (new line, no pseudocode before)
    //append_to_array(tmp, &array_len, "return values_arr[mid_index]");
    //append_to_array(tmp, &array_len, "jmp left_block  + 1"); //is len(...)
    //append_to_array(tmp, &array_len, l_mem);
    //append_to_array(tmp, &array_len, r_mem);
    
    *memory = tmp;
    return array_len;
}

int main(int argc, char* argv[]) {
  if (argc < 2)
    return 1;
  uint64_t res = f(atoi(argv[1]));
  printf("res = %"SCNu64"\n", res);
  return 0;
}
