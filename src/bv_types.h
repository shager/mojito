#ifndef BV_TYPES_H
#define BV_TYPES_H 1

#define INIT_SIZE 4

#include "bv_list.h"
#include <stdint.h>

typedef struct Bitvector {
    uint64_t* bitvector;
    uint32_t bitvector_length;
    
    int (*insert_rule_at_position)(struct Bitvector*, uint32_t, uint8_t);
    int (*delete_rule_from_position)(struct Bitvector*, uint32_t);
    int (*merge_bitvectors)(struct Bitvector*, struct Bitvector*);
} Bitvector;

struct Bitvector* bitvector_ctor();
void bitvector_dtor(struct Bitvector* this);

int Bv_insert_rule_at_position(struct Bitvector* this, uint32_t position, uint8_t value);
int Bv_delete_rule_from_position(struct Bitvector* this, uint32_t position);
int Bv_merge_bitvectors(struct Bitvector* first, struct Bitvector* second);

typedef struct Delimiter {
    uint32_t delimiter_value;

    struct Bitvector* bitvector;
    //struct Bv_list* rule_list;
} Delimiter;

typedef struct Range_borders {
    uint32_t range_borders_max; //track what max index the array has at the moment
    uint32_t range_borders_current; //track the current first unused element
    struct Delimiter *range_borders; //array of struct Delimiter*
 
    int (*insert_element)(struct Range_borders*, struct Delimiter*);
    int (*insert_element_at_index)(struct Range_borders*, struct Delimiter*, uint32_t);
    int (*delete_element)(struct Range_borders*, uint32_t);
    int (*add_rule)(struct Range_borders*, uint32_t, uint32_t, uint32_t);
    int64_t (*find_element)(struct Range_borders*, uint32_t);
    uint8_t (*match_packet)(struct Range_borders*, struct Bitvector**, uint32_t);
} Range_borders;

struct Range_borders* range_borders_ctor();
void range_borders_dtor(struct Range_borders* this);

int Rb_insert_element(struct Range_borders* this, struct Delimiter* new_element);
int Rb_insert_element_at_index(struct Range_borders* this, struct Delimiter* new_element, uint32_t index);
int Rb_delete_element(struct Range_borders* this, uint32_t index);
int Rb_add_rule(struct Range_borders* this, uint32_t begin_index, uint32_t end_index, uint32_t rule_index);
int64_t Rb_find_element(struct Range_borders* this, uint32_t value);
uint8_t Rb_match_packet(struct Range_borders* this, struct Bitvector** result, uint32_t header_value);

int64_t binary_search(struct Range_borders* this, uint32_t value, uint32_t lower, uint32_t upper);
uint32_t find_free_position(struct Range_borders* this, uint32_t target);

#endif //BV_TYPES_H
