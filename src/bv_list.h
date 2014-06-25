#ifndef BV_LIST_H
#define BV_LIST_H 1

#include <stdint.h>

typedef struct Bv_list {
    uint32_t rule_index;
    struct Bv_list* next;
    
    uint8_t (*append)(struct Bv_list*, uint32_t);
    uint8_t (*is_empty)(struct Bv_list*);
} Bv_list;

struct Bv_list* list_ctor();
void list_dtor(struct Bv_list*);

uint8_t L_append(struct Bv_list* this, uint32_t new_element);
uint8_t L_is_empty(struct Bv_list* this); //TODO: boolean retval?

#endif //BV_LIST_H
