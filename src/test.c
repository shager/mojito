#include "bv_types.h"
#include <stdio.h>

int main() {
    Range_borders* obj = range_borders_ctor();
    // test rule-adding function
    printf("Adding rules:\n");
    for (int i = 0; i < 1000; ++i) {
        if (obj->add_rule(obj, i * 100, i * 100 + 1000, i * 10) != 0) {
            printf("Error adding rule %d\n", i);
        }
    }
    
    printf("done.\n\nMatching packets:\n");
    Bv_list* result_list = list_ctor();
    if (obj->match_packet(obj, result_list, 170) != 0)
        printf("Error matching packet 170\n");
    
    printf("Matching rules are: ");
    while (result_list->next != NULL) {
        printf("%d ", result_list->rule_index);
        result_list = result_list->next;
    }
    
    printf("\n");
    return 0;
    
    /*printf("%s\n", "Inserting numbers:");
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
    
    printf("\n");
    
    printf("%s\n", "Searching for number 53:");
    printf("Found at index %d\n", obj->find_element(obj, 53));
    
    printf("\n%s", "Freeing memory... ");
    range_borders_dtor(obj);
    printf("%s\n", "Success.");
    
    printf("%s\n", "..... next test: insert at index:");
    Range_borders* obj2 = range_borders_ctor();
    for (int i = 0; i < 100; i++){
        printf("%d ", i);
        if (obj2->insert_element_at_index(obj2, i, i) == 1) {
            printf("%s", "Allocation error at inserting numbers, aborting...");
            return 1;
        }
    }*/
    
    /*printf("\n%s", "Inserting number 42 all 5 positions... ");
    for (int i = 0; i < 100; i+=5){
        obj2->insert_element_at_index(obj2, 42, i);
    }*/
    
    /*printf("\n");
    
    printf("%s\n", "Extracting numbers:");
    for (int i = 0; i < obj2->range_borders_current; ++i) {
        printf("%d ", obj2->range_borders[i]);
    }
    
    printf("\n\n");
    
    printf("%s\n", "Searching for number 42:");
    printf("Found at index %d\n", obj2->find_element(obj2, 42));
    
    printf("\n");
    
    printf("%s\n", "Searching for number 1337:");
    printf("Found at index %d\n", obj2->find_element(obj2, 1337));
    
    
    return 0;*/
}
