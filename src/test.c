#include "bv_types.h"
#include <stdio.h>

int main() {
    Range_borders* obj = range_borders_ctor();
    // test insert function
    for (int i = 0; i < 100; ++i) {
        printf("%d\n", i);
        obj->insert_element(obj, i);
    }
    
    for (int i = 0; i < 100; ++i) {
        if (obj->range_borders[i] == i)
            continue;
        else {
            printf("%s%d%s\n", "Failure at index", i, "!");
            return 1;
        }
    }
    printf("%s\n", "Success.");
    return 0;
}
