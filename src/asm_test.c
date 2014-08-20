#include "asm.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    uint64_t values[] = {1, 3, 5, 8, 11, 13};
    
    char* code = NULL;
    uint32_t size = construct_node(values, 6, 0, 5, &code);
    
    void* mem = mmap(NULL, size, PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    memcpy(mem, code, size);
    
    printf("Done\n");
    free(code);
    return 0;
}
