#include <stdio.h>
#include <assert.h>
#include "asm.h"

//append value to array
void append_to_array(char** array, uint32_t* array_len, char* value, uint32_t value_len) {
    if (value_len == 0)
        return;
    
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
void mov(char** mem, uint32_t* mem_len, uint32_t target) {
    char* opcode_buffer = malloc(5 * sizeof(char));
    opcode_buffer[0] = 0xb8; //mov $target,%eax
    //next 4 bytes are the number target (in little endian)
    opcode_buffer[1] = (char)(target & 0xFF);
    opcode_buffer[2] = (char)((target >> 8) & 0xFF);
    opcode_buffer[3] = (char)((target >> 16) & 0xFF);
    opcode_buffer[4] = (char)((target >> 24) & 0xFF);
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
    opcode_buffer[4] = (char)(x & 0xFF);
    opcode_buffer[5] = (char)((x >> 8) & 0xFF);
    opcode_buffer[6] = (char)((x >> 16) & 0xFF);
    opcode_buffer[7] = (char)((x >> 24) & 0xFF);
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
    opcode_buffer[3] = (char)((x >> 8) & 0xFF);
    opcode_buffer[4] = (char)((x >> 16) & 0xFF);
    opcode_buffer[5] = (char)((x >> 24) & 0xFF);
    opcode_buffer[6] = (char)((x >> 32) & 0xFF);
    opcode_buffer[7] = (char)((x >> 40) & 0xFF);
    opcode_buffer[8] = (char)((x >> 48) & 0xFF);
    opcode_buffer[9] = (char)((x >> 56) & 0xFF);
    opcode_buffer[10] = 0x48; //cmp %rax,-0x8(%rbp)
    opcode_buffer[11] = 0x39;
    opcode_buffer[12] = 0x45;
    opcode_buffer[13] = 0xf8;
    append_to_array(mem, mem_len, opcode_buffer, 14);
    free(opcode_buffer);
}

//Jump if below
void jb(char** mem, uint32_t* mem_len, uint32_t offset) {
    char* opcode_buffer = malloc(6 * sizeof(char));
    opcode_buffer[0] = 0x0f; //jb by offset bytes
    opcode_buffer[1] = 0x82;
    //next 4 bytes are the offset (in little endian)
    opcode_buffer[2] = (char)(offset & 0xFF);
    opcode_buffer[3] = (char)((offset >> 8) & 0xFF);
    opcode_buffer[4] = (char)((offset >> 16) & 0xFF);
    opcode_buffer[5] = (char)((offset >> 24) & 0xFF);
    append_to_array(mem, mem_len, opcode_buffer, 6);
    free(opcode_buffer);
}

//Jump if above or equal
void jae(char** mem, uint32_t* mem_len, uint32_t offset) {
    char* opcode_buffer = malloc(6 * sizeof(char));
    opcode_buffer[0] = 0x0f; //jae by offset bytes
    opcode_buffer[1] = 0x83;
    //next 4 bytes are the offset (in little endian)
    opcode_buffer[2] = (char)(offset & 0xFF);
    opcode_buffer[3] = (char)((offset >> 8) & 0xFF);
    opcode_buffer[4] = (char)((offset >> 16) & 0xFF);
    opcode_buffer[5] = (char)((offset >> 24) & 0xFF);
    append_to_array(mem, mem_len, opcode_buffer, 6);
    free(opcode_buffer);
}

uint32_t safe_cmpq(char** mem, uint32_t* mem_len, uint64_t x) {
    if ((x & 0xFFFFFFFF) == x) {
        cmpq(mem, mem_len, x);
        return 8;
    } else {
        mov_and_cmpq(mem, mem_len, x);
        return 14;
    }
}

/*
 * values_arr: Array with values in which the search is done
 * values_arr_length: length of the array in bytes
 * low_index: lower bound for search
 * hi_index: upper bound for search
 * memory: byte array used for returning instructions
 * 
 * returns: length of memory in bytes
 */
uint32_t construct_node(uint64_t* values_arr, uint32_t values_arr_length, uint32_t low_index, uint32_t hi_index, char** memory) {    
    uint32_t mid_index = low_index + ((hi_index - low_index) / 2);
    
    char* tmp = NULL;
    char* l_mem = NULL;
    char* r_mem = NULL;
    
    uint32_t memory_len = 0;
    
    //execute at first run
    if (low_index == 0 && hi_index == values_arr_length - 1) {
        push(&tmp, &memory_len);
        mov_stack(&tmp, &memory_len);
    }

    //base cases
    if ((hi_index - low_index) == -1) {
        memory = NULL;
        return 0;
    }
    if ((hi_index - low_index) == 0) {
        safe_cmpq(&tmp, &memory_len, values_arr[low_index]);
        jb(&tmp, &memory_len, 0x7); //offset = len(mov) + len(pop_ret)
        
        //return values_arr[low_index]
        mov(&tmp, &memory_len, (uint32_t)values_arr[low_index]);
        pop_ret(&tmp, &memory_len);
        
        if (low_index == 0) {
            //return NULL
            mov(&tmp, &memory_len, 0x0);
            pop_ret(&tmp, &memory_len);
        } else {
            //return element left of current one
            mov(&tmp, &memory_len, (uint32_t)values_arr[low_index - 1]);
            pop_ret(&tmp, &memory_len);
        }
        
        *memory = tmp;
        return memory_len;
    }
    
    uint32_t left_block = construct_node(values_arr, values_arr_length, low_index, mid_index - 1, &l_mem);
    uint32_t right_block = construct_node(values_arr, values_arr_length, mid_index + 1, hi_index, &r_mem);
    
    //Get length of safe_cmpq for jb to LEFT
    uint32_t len_safe_cmpq = 0;
    if ((values_arr[mid_index + 1] & 0xFFFFFFFF) == values_arr[mid_index + 1]) {
        len_safe_cmpq = 8;
    } else {
        len_safe_cmpq = 14;
    }
    
    //main block
    // cmpq x < values_arr[mid_index]
    safe_cmpq(&tmp, &memory_len, values_arr[mid_index]);
    // Jb LEFT
    jb(&tmp, &memory_len, len_safe_cmpq + 6 + 5 + 2); //offset = len_safe_cmpq + len(jae) + len(mov) + len(pop_ret)
    // cmpq values_arr[mid_index + 1] < x
    safe_cmpq(&tmp, &memory_len, values_arr[mid_index + 1]);
    // Ja RIGHT
    jae(&tmp, &memory_len, 5 + 2 + left_block); //len = 3, offset = len(mov) + len(pop_ret)
    // return values_arr[mid_index]
    mov(&tmp, &memory_len, (uint32_t)values_arr[mid_index]);
    pop_ret(&tmp, &memory_len);
    append_to_array(&tmp, &memory_len, l_mem, left_block); //LEFT
    append_to_array(&tmp, &memory_len, r_mem, right_block); //RIGHT
    
    free(l_mem);
    free(r_mem);
    
    *memory = tmp;
    return memory_len;
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < 1000; i++) {
        char* code = NULL;
        uint64_t test_array[10 * i];
        for (int j = 0; j < 10 * i; j++) {
            test_array[j] = 5 * j;
        }
        uint32_t size = construct_node(test_array, 10 * i, 0, (10 * i) - 1, &code);
        void* mem = mmap(NULL, size, PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        memcpy(mem, code, size);
        free(code);
 
        uint32_t (*f)(uint64_t value) = mem;
        
        for (int k = 0; k < 5; k++) {
            for (int j = 0; j < i; j++) {
                uint32_t res = f(k + (j * 5));
                assert(res == (j * 5));
            }
         }
        
        munmap(mem, size);
    }
    printf("done\n");
    return 0;
}
