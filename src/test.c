#include "bv_types.h"
#include <stdio.h>
#include <assert.h>

int main() {
    Range_borders* obj = range_borders_ctor();
    
    /* TEST:
     * find_free_position(struct Range_borders* this, uint64_t target) 
     */
     
    /*obj->add_rule(obj, 1, 10, 2);
    obj->add_rule(obj, 999, 2900, 7);
    obj->add_rule(obj, 1000, 2800, 1);
    Bitvector* res;
    obj->match_packet(obj, &res, 7);
    
    printf("res = %lld\n", (long long unsigned int)(res->bitvector[0]));
    assert((res->bitvector[0] >> 61) == 1);
    
    Bitvector* res2;
    obj->match_packet(obj, &res2, 1500);
    printf("res = %lld\n", (long long unsigned int)(res2->bitvector[0]));
    assert((res2->bitvector[0] >> 62) % 2 == 1);
    
    Bitvector* res3;
    obj->match_packet(obj, &res3, 2888);
    printf("res = %lld\n", (long long unsigned int)(res3->bitvector[0]));
    assert((res3->bitvector[0] >> 56) == 1);
    
    //return 0;
    assert((find_free_position(obj, 0) == 0) || printf(" find_free_position failed!\n"));*/
    
    /* 
     * test add_rule(struct Range_borders* this, uint64_t begin_index, uint64_t end_index, uint32_t rule_index)
     */
    for (int i = 0; i < 100; ++i) {
        printf("Adding rule %d\n", i);
        assert((obj->add_rule(obj, i, i + 10, i) == 0) || printf("Error adding rule %d\n", i));
    }
    
    printf("done.\n\nMatching packets:\n");
    Bitvector* result_bv;
    if (obj->match_packet(obj, &result_bv, 107) != 0)
        printf("Error matching packet 107\n");
    
    printf("Matching rules are: ");
    printf("%lld\n", (long long unsigned int)result_bv->bitvector[0]);
    
    range_borders_dtor(obj);
    
    printf("\ndone!\n");
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
