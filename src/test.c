#include "bv_types.h"
#include <stdio.h>

int main() {
    // test insert function
    for (int i = 0; i < 100; ++i) {
        printf("%d\n", i);
        insertElement(rangeBorders, i);
    }
    
    for (int i = 0; i < 100; ++i) {
        if (rangeBorders[i] == i)
            continue;
        else {
            printf("%s%d%s\n", "Failure at index", i, "!");
            return 1;
        }
    }
    printf("%s\n", "Success.");
    return 0;
}
