#include <stdlib.h>
#include <stdio.h>
#include "bv_types.h"

struct Range_borders* range_borders_ctor() {
    Range_borders* object = (Range_borders*) calloc(1, sizeof(Range_borders));
    if (object == NULL)
        return NULL;
    object->range_borders = (int*) calloc(INIT_SIZE, sizeof(int));
    if (object->range_borders == NULL)
        return NULL;
    object->range_borders_max = INIT_SIZE;
    object->range_borders_current = 0;
    object->insert_element = Rb_insert_element;
    object->delete_element = Rb_delete_element;
    return object;
}

void range_borders_dtor(struct Range_borders* this) {
    free(this->range_borders);
    free(this);
}

int Rb_insert_element(struct Range_borders* this, int new_element) {
    if (this->range_borders_current < this->range_borders_max) {
        this->range_borders[this->range_borders_current++] = new_element;
    } else {
        this->range_borders_max *= 2;
        int* tmp = (int*)realloc(this->range_borders, (this->range_borders_max) * sizeof(int));
        if (tmp != NULL) {
            this->range_borders = tmp;
        } else {
            free(tmp);
            return 1;
        }
        this->range_borders[this->range_borders_current++] = new_element;
    }
    return 0;
}

int Rb_delete_element(struct Range_borders* this, int index) {
    // check if index is in array
    if (index > this->range_borders_current - 1) {
        return 1;
    }
    for (int i = index; i < this->range_borders_current; ++i) {
        this->range_borders[i] = this->range_borders[i + 1];
    }
    this->range_borders[--this->range_borders_current] = 0;

    // check if we can resize the array to half
    if (this->range_borders_current < this->range_borders_max / 2) {
        this->range_borders_max /= 2;
        int* tmp = (int*)realloc(this->range_borders, (this->range_borders_max) * sizeof(int));
        if (tmp != NULL) {
            this->range_borders = tmp;
        } else {
            free(tmp);
            return 1;
        }
    }
    return 0;
}
