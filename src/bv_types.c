#include <stdlib.h>
#include <stdio.h>
#include "bv_types.h"

struct Bitvector* bitvector_ctor() {
    Bitvector* object = (Bitvector*) calloc(1, sizeof(Bitvector));
    if (object == NULL)
        return NULL;
    
    object->bitvector_length = 0;
    object->bitvector = calloc(1, sizeof(uint64_t));
    if (object->bitvector == NULL)
        return NULL;
        
    object->insert_rule_at_position = Bv_insert_rule_at_position;
    object->delete_rule_from_position = Bv_delete_rule_from_position;
    object->merge_bitvectors = Bv_merge_bitvectors;
    
    return object;
}

void bitvector_dtor(struct Bitvector* this) {
    free(this->bitvector);
}

int Bv_insert_rule_at_position(struct Bitvector* this, uint32_t position, uint8_t value) {
    const int stepsize = (sizeof(uint64_t) * 8); // stepsize = 64
    if (this->bitvector_length % stepsize == 0) {
        this->bitvector = (uint64_t*)realloc(this->bitvector, sizeof(uint64_t) * (this->bitvector_length / stepsize) + 1);
    }
    
    this->bitvector_length++;
    
    // find out which element in the array has to be altered
    uint32_t pos_in_array = position / stepsize;
    
    // right-shift bits at and after insert-position ("blockwise")
    int last_bit = this->bitvector[pos_in_array] & 0x1;
    for (int i = pos_in_array + 1; i <= ((this->bitvector_length / stepsize) + 1); ++i) {
        int _last_bit = this->bitvector[i] & 0x1;
        this->bitvector[i] >>= 1;
        this->bitvector[i] |= (last_bit << (sizeof(int) - 1));
        last_bit = _last_bit;
    }
    //TODO: insert bit, shift right in respective block
    
    return 0;
}

int Bv_delete_rule_from_position(struct Bitvector* this, uint32_t position) {
    return 0;
}

// merges Bitvectors first and second into second
int Bv_merge_bitvectors(struct Bitvector* first, struct Bitvector* second) {
    const int stepsize = (sizeof(uint64_t) * 8);
    for (int i = 0; i <= first->bitvector_length / stepsize; ++i) {
        second->bitvector[i] = second->bitvector[i] & first->bitvector[i];
    }
    return 0;
}

struct Range_borders* range_borders_ctor() {
    Range_borders* object = (Range_borders*) calloc(1, sizeof(Range_borders));
    if (object == NULL)
        return NULL;
    
    object->range_borders = (Delimiter*) calloc(INIT_SIZE, sizeof(Delimiter));
    if (object->range_borders == NULL)
        return NULL;
    for (int i = 0; i < INIT_SIZE; ++i) {
        object->range_borders[i].delimiter_value = 0;
        object->range_borders[i].bitvector = bitvector_ctor();
    }
    object->range_borders_max = INIT_SIZE;
    object->range_borders_current = 0;
    object->insert_element = Rb_insert_element;
    object->insert_element_at_index = Rb_insert_element_at_index;
    object->delete_element = Rb_delete_element;
    object->add_rule = Rb_add_rule;
    object->find_element = Rb_find_element;
    object->match_packet = Rb_match_packet;
    
    return object;
}

void range_borders_dtor(struct Range_borders* this) {
    for (int i = 0; i < this->range_borders_current; ++i) {
        bitvector_dtor(this->range_borders[i].bitvector);
    }
    free(this->range_borders);
    free(this);
}

// Insert one element into the array
int Rb_insert_element(struct Range_borders* this, struct Delimiter* new_element) {
    if (this->range_borders_current < this->range_borders_max) {
        this->range_borders[this->range_borders_current++] = *new_element;
    } else {
        this->range_borders_max *= 2;
        Delimiter* tmp = (Delimiter*)realloc(this->range_borders, (this->range_borders_max) * sizeof(Delimiter));
        if (tmp != NULL) {
            this->range_borders = tmp;
        } else {
            free(tmp);
            return 1;
        }
        this->range_borders[this->range_borders_current++] = *new_element;
    }
    return 0;
}

// Inserts an element at specified index. If index is greater or equal 
// range_borders_current, the new_element is inserted in the last position of the array
int Rb_insert_element_at_index(struct Range_borders* this, struct Delimiter* new_element, uint32_t index) {
    // check if the insertion happens in a new slot at the end of the array
    //printf("Entered insert at index %d...\n", index);
    //fflush(stdout);
    if (index >= this->range_borders_current) {
        this->insert_element(this, new_element);
        return 0;
    } else { // insertion happens somewhere "in the middle"
        this->insert_element(this, &(this->range_borders[this->range_borders_current - 1]));
        for (uint32_t i = this->range_borders_current - 2; i > index; --i){ // was >=
            //printf("in loop, i = %d\n", i);
            fflush(stdout);
            this->range_borders[i + 1] = this->range_borders[i];
        }
        this->range_borders[index] = *new_element;
    }
    //printf("done\n");
    return 0;
}

// Delete a single element in the array
int Rb_delete_element(struct Range_borders* this, uint32_t index) {
    // check if index is in array
    if (index > this->range_borders_current - 1) {
        return 1;
    }
    
    for (int i = index; i < this->range_borders_current; ++i) {
        this->range_borders[i] = this->range_borders[i + 1];
    }

    bitvector_dtor(this->range_borders[this->range_borders_current - 1].bitvector);
    free(&(this->range_borders[--this->range_borders_current]));

    // check if we can resize the array to half
    if (this->range_borders_current < this->range_borders_max / 2) {
        this->range_borders_max /= 2;
        Delimiter* tmp = (Delimiter*)realloc(this->range_borders, (this->range_borders_max) * sizeof(Delimiter));
        if (tmp != NULL) {
            this->range_borders = tmp;
        } else {
            free(tmp);
            return 1;
        }
    }
    return 0;
}

// Insert a new rule into the routing table
int Rb_add_rule(struct Range_borders* this, uint32_t begin_index, uint32_t end_index, uint32_t rule_index) {
    int position_begin = find_free_position(this, begin_index); //TODO: check if still right with -1
    
    for (int i = 0; i < position_begin; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(this->range_borders[i].bitvector, rule_index, 0);
    }
    
    // insert new range_border entry, if it doesn't exist
    if (begin_index == this->range_borders[position_begin].delimiter_value) {
        this->range_borders[position_begin].bitvector->insert_rule_at_position(this->range_borders[position_begin].bitvector, rule_index, 1);
    } else {
        struct Delimiter* new_begin_entry = malloc(sizeof(Delimiter));
        if (new_begin_entry == NULL)
            return 1;
        new_begin_entry->delimiter_value = begin_index;
        
        struct Bitvector* begin_bitvector = bitvector_ctor();
        begin_bitvector->insert_rule_at_position(begin_bitvector, rule_index, 1);
        
        // append all rules from previous entry here
        begin_bitvector->merge_bitvectors(this->range_borders[position_begin - 1].bitvector, begin_bitvector);
        
        new_begin_entry->bitvector = begin_bitvector;
        Rb_insert_element_at_index(this, new_begin_entry, position_begin);
    }
    
    // determine end of insertion area
    int position_end = find_free_position(this, end_index) - 1; //TODO: check if -1 still works
    
    for (int i = position_begin + 1; i < position_end; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(this->range_borders[i].bitvector, rule_index, 1);
    }
    
    //insert new range_border entry, if not exists
    if (end_index == this->range_borders[position_end].delimiter_value) {
        this->range_borders[position_begin].bitvector->insert_rule_at_position(this->range_borders[position_end].bitvector, rule_index, 0);
    } else {
        struct Delimiter* new_end_entry = malloc(sizeof(Delimiter));
        if (new_end_entry == NULL)
            return 1;
        new_end_entry->delimiter_value = end_index;
        
        struct Bitvector* end_bitvector = bitvector_ctor();
        end_bitvector->insert_rule_at_position(end_bitvector, rule_index, 0);
        
        // append all rules from previous entry here
        end_bitvector->merge_bitvectors(this->range_borders[position_end - 1].bitvector, end_bitvector);
        
        new_end_entry->bitvector = end_bitvector;
        Rb_insert_element_at_index(this, new_end_entry, position_end);
    }
    
    for (int i = position_end + 1; i < this->range_borders_current; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(this->range_borders[i].bitvector, rule_index, 0);
    }
    return 0;
}

// Binary search, returns index
int64_t Rb_find_element(struct Range_borders* this, uint32_t value) {
    if (this->range_borders_current == 0)
        return -1;
    return binary_search(this, value, 0, this->range_borders_current - 1);
}

// Match a header field value of an incoming packet
uint8_t Rb_match_packet(struct Range_borders* this, struct Bitvector** result, uint32_t header_value) {
    struct Bitvector* tmp = bitvector_ctor();
    int relevant_border = find_free_position(this, header_value);
    if (relevant_border == -1) {
        result = NULL;
        return 0;
    }
    
    tmp = this->range_borders[relevant_border].bitvector;
    *result = tmp;
    return 0;
}

int64_t binary_search(struct Range_borders* this, uint32_t value, uint32_t lower, uint32_t upper) {
    if (upper < lower)
        return -1;
    
    int mid_index = lower + ((upper - lower) / 2);
    int current = this->range_borders[mid_index].delimiter_value;
    if (current > value)
        return binary_search(this, value, lower, mid_index - 1);
    else if (current < value)
        return binary_search(this, value, mid_index + 1, upper);
        
    return mid_index;
}

uint32_t find_free_position(struct Range_borders* this, uint32_t target) {
    int lower = 0;
    int upper = this->range_borders_current - 1;
    if (upper == -1)
        return upper;
    while (lower != upper) {
        int mid_index = lower + ((upper - lower) / 2);
        if (this->range_borders[mid_index].delimiter_value <= target) {
            lower = mid_index + 1;
        } else {
            upper = mid_index;
        }
    }
    return lower;
}
