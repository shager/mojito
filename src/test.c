#include "bv_types.h"
#include <stdio.h>

int main() {
    Range_borders* obj = range_borders_ctor();
    // test insert function
    printf("%s\n", "Inserting numbers:");
    for (int i = 0; i < 130; ++i) {
        printf("%d ", i);
        if (obj->insert_element(obj, i) == 1) {
            printf("%s", "Allocation error at inserting numbers, aborting...");
            return 1;
        }
    }
    printf("\n");
    
    printf("%s\n", "Extracting numbers:");
    for (int i = 0; i < obj->range_borders_current; ++i) {
        if (obj->range_borders[i] == i) {
            printf("%d ", i);
            continue;
        } else {
            printf("%s%d%s\n", "Failure at index", i, "!");
            return 1;
        }
    }
    printf("\n");
    
    printf("%s\n", "Deleting even numbers...");
    for (int i = 0; i < obj->range_borders_current; ++i) {
        if (obj->delete_element(obj, i) == 1) {
            printf("%s%d%s", "Allocation error at index ", i, ", aborting...\n");
            return 1;
        }
    }
    
    printf("%s\n", "Extracting numbers:");
    for (int i = 0; i < obj->range_borders_current; ++i) {
        printf("%d ", obj->range_borders[i]);
    }
    
    printf("\n%s", "Freeing memory... ");
    range_borders_dtor(obj);
    printf("%s\n", "Success.");
    return 0;
}
