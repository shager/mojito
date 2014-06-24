#include "bv_list.h"

struct Bv_list* list_ctor() {
    Bv_list* object = (Bv_list*) calloc(1, sizeof(Bv_list));
    if (object == NULL)
        return NULL;

    object->rule_index = 0;
    object->next = NULL;
    object->append = L_append;
    object->is_empty = L_is_empty;
    
    return object;
}

void list_dtor(struct Bv_list* this) {
    //TODO
}

uint8_t L_append(struct Bv_list* this, uint32_t new_element) {
    Bv_list* new_object = (Bv_list*) calloc(1, sizeof(Bv_list));
    if (new_object == NULL)
        return 1;
    new_object->rule_index = new_element;
    new_object->next = NULL;
    
    this->next = new_object;
    return 0;
}

// Returns 0 if list is empty
uint8_t L_is_empty(struct Bv_list* this) {
    if (this->next == NULL)
        return 0;

    return 1;
}
