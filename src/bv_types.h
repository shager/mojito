#ifndef BV_TYPES_H
#define BV_TYPES_H 1

#define INIT_SIZE 4

#include <stdint.h>


//TODO: support 64bit operation (e.g. in insertion of values)
typedef struct Bitvector {
    uint64_t* bitvector;
    uint32_t bitvector_length;
    
    int (*insert_rule_at_position)(struct Bitvector*, uint32_t, uint8_t);
    int (*delete_rule_from_position)(struct Bitvector*, uint32_t);
    int (*merge_bitvectors)(struct Bitvector*, struct Bitvector*);
} Bitvector;

Bitvector* bitvector_ctor();
void bitvector_dtor(Bitvector* this);

int Bv_insert_rule_at_position(Bitvector* this, uint32_t position, uint8_t value);
int Bv_delete_rule_from_position(Bitvector* this, uint32_t position);
int Bv_merge_bitvectors(Bitvector* first, Bitvector* second);

typedef struct Delimiter {
    uint64_t delimiter_value;

    Bitvector* bitvector;
} Delimiter;

typedef struct Range_borders {
    uint32_t range_borders_max; //track what max index the array has at the moment
    uint32_t range_borders_current; //track the current first unused element
    Delimiter* range_borders; //array of Delimiter*
 
    int (*insert_element)(struct Range_borders*, Delimiter*);
    int (*insert_element_at_index)(struct Range_borders*, Delimiter*, uint32_t);
    int (*delete_element)(struct Range_borders*, uint32_t);
    int (*add_rule)(struct Range_borders*, uint64_t, uint64_t, uint32_t);
    int64_t (*find_element)(struct Range_borders*, uint32_t);
    uint8_t (*match_packet)(struct Range_borders*, Bitvector**, uint64_t);
    
    //JIT lookup function
    uint32_t (*jit_lookup)(uint64_t);
    uint32_t jit_lookup_size;
} Range_borders;

Range_borders* range_borders_ctor();
void range_borders_dtor(Range_borders* this);

int Rb_insert_element(Range_borders* this, Delimiter* new_element);
int Rb_insert_element_at_index(Range_borders* this, Delimiter* new_element, uint32_t index);
int Rb_delete_element(Range_borders* this, uint32_t index);
int Rb_add_rule(Range_borders* this, uint64_t begin_index, uint64_t end_index, uint32_t rule_index);
int Rb_add_rule_jit(Range_borders* this, uint64_t begin_index, uint64_t end_index, uint32_t rule_index);
int64_t Rb_find_element(Range_borders* this, uint32_t value);
uint8_t Rb_match_packet(Range_borders* this, Bitvector** result, uint64_t header_value);
uint8_t Rb_match_packet_jit(Range_borders* this, Bitvector** result, uint64_t header_value);

int64_t binary_search(Range_borders* this, uint32_t value, uint32_t lower, uint32_t upper);
uint32_t find_free_position(Range_borders* this, uint64_t target);

#endif //BV_TYPES_H
