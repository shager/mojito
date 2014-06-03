#include <stdlib.h>
#include "bv_types.h"

struct Range_borders* range_borders_ctor() {
    Range_borders* object = (Range_borders*) malloc(sizeof(Range_borders));
    object->range_borders_max = INIT_SIZE;
    object->range_borders_current = 0;
    object->insert_element = Rb_insert_element;
    object->delete_element = Rb_delete_element;
    return object;
}

int Rb_insert_element(struct Range_borders_t* this, int new_element) {
    if (this->range_borders_current < this->range_borders_max) {
        this->range_borders[this->range_borders_current++] = new_element;
    } else {
        this->range_borders_max *= 2;
        this->range_borders = (int*)realloc(this->range_borders, (this->range_borders_max) * sizeof(int));
        this->range_borders[this->range_borders_current++] = new_element;
    }
    return 0;
}

int Rb_delete_element(struct Range_borders_t* this, int index) {
    if (sizeof(this->range_borders) / sizeof(this->range_borders[0]) > index - 1)
        return 1;
    for (int i = index; i < this->range_borders_current; ++i) {
        this->range_borders[i] = this->range_borders[i + 1];
    }
    this->range_borders[this->range_borders_current--] = 0;
    
    // check if we can resize the array to half
    if (this->range_borders_current < this->range_borders_max / 2) {
        this->range_borders_max /= 2;
        this->range_borders = (int*)realloc(this->range_borders, (this->range_borders_max) * sizeof(int));
    }
    return 0;
}
