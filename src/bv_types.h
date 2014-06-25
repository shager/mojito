#ifndef BV_TYPES_H
#define BV_TYPES_H 1

#define INIT_SIZE 4

#include "bv_list.h"
#include <stdint.h>

typedef struct Delimiter {
    uint32_t delimiter_value;
    struct Bv_list* rule_list;
} Delimiter;

typedef struct Range_borders {
    uint32_t range_borders_max; //track what max index the array has at the moment
    uint32_t range_borders_current; //track the current first unused element
    Delimiter *range_borders;
 
    int (*insert_element)(struct Range_borders*, struct Delimiter*);
    int (*insert_element_at_index)(struct Range_borders*, struct Delimiter*, uint32_t);
    int (*delete_element)(struct Range_borders*, uint32_t);
    int (*add_rule)(struct Range_borders*, uint32_t, uint32_t, uint32_t);
    uint64_t (*find_element)(struct Range_borders*, uint32_t);
    uint64_t (*match_packet)(struct Range_borders*, uint32_t);
} Range_borders;

struct Range_borders* range_borders_ctor();
void range_borders_dtor(struct Range_borders* this);

int Rb_insert_element(struct Range_borders* this, struct Delimiter* new_element);
int Rb_insert_element_at_index(struct Range_borders* this, struct Delimiter* new_element, uint32_t index);
int Rb_delete_element(struct Range_borders* this, uint32_t index);
int Rb_add_rule(struct Range_borders* this, uint32_t begin_index, uint32_t end_index, uint32_t rule_index);
uint64_t Rb_find_element(struct Range_borders* this, uint32_t value);
uint64_t Rb_match_packet(struct Range_borders* this, uint32_t header_value);

uint64_t binary_search(struct Range_borders* this, uint32_t value, uint32_t lower, uint32_t upper);
uint32_t find_free_position(struct Range_borders* this, uint32_t target);

#endif //BV_TYPES_H
