#ifndef BV_TYPES_H
#define BV_TYPES_H 1

#define INIT_SIZE 4

#include "bv_list.h"

typedef struct Delimiter {
    int delimiter_value;
    struct Bv_list* rule_list;
} Delimiter;

typedef struct Range_borders {
    int range_borders_max; //track what max index the array has at the moment
    int range_borders_current; //track the current first unused element
    int *range_borders;
 
    int (*insert_element)(struct Range_borders*, int);
    int (*insert_element_at_index)(struct Range_borders*, int, int);
    int (*delete_element)(struct Range_borders*, int);
    int (*add_rule)(struct Range_borders*, int, int, int);
    int (*find_element)(struct Range_borders*, int);
    int (*match_packet)(struct Range_borders*, int);
} Range_borders;

struct Range_borders* range_borders_ctor();
void range_borders_dtor(struct Range_borders* this);

int Rb_insert_element(struct Range_borders* this, int new_element);
int Rb_insert_element_at_index(struct Range_borders* this, int new_element, int index);
int Rb_delete_element(struct Range_borders* this, int index);
//TODO: Datentyp anders als int? muss bitvector sein!
int Rb_add_rule(struct Range_borders* this, int begin_index, int end_index, int rule_index);
int Rb_find_element(struct Range_borders* this, int value);
int Rb_match_packet(struct Range_borders* this, int header_value);

int binary_search(struct Range_borders* this, int value, int lower, int upper);
int find_free_position(struct Range_borders* this, int target);

#endif //BV_TYPES_H
