#include <stdlib.h>
#include <stdio.h>
#include "bv_types.h"

struct Range_borders* range_borders_ctor() {
    Range_borders* object = (Range_borders*) calloc(1, sizeof(Range_borders));
    if (object == NULL)
        return NULL;
    
    object->range_borders = (Delimiter*) calloc(INIT_SIZE, sizeof(Delimiter));
    if (object->range_borders == NULL)
        return NULL;
    for (int i = 0; i < INIT_SIZE; ++i) {
        object->range_borders[i]->delimiter_value = 0;
        object->range_borders[i]->rule_list = list_ctor();
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

// TODO: Leaks here? (Delimiter lists)
void range_borders_dtor(struct Range_borders* this) {
    free(this->range_borders);
    free(this);
}

// Insert one element into the array
int Rb_insert_element(struct Range_borders* this, struct Delimiter* new_element) {
    if (this->range_borders_current < this->range_borders_max) {
        this->range_borders[this->range_borders_current++] = new_element;
    } else {
        this->range_borders_max *= 2;
        Delimiter* tmp = (Delimiter*)realloc(this->range_borders, (this->range_borders_max) * sizeof(Delimiter));
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

// Inserts an element at specified index. If index is greater or equal 
// range_borders_current, the new_element is inserted in the last position of the array
int Rb_insert_element_at_index(struct Range_borders* this, struct Delimiter* new_element, uint32_t index) {
    // check if the insertion happens in a new slot at the end of the array
    if (index >= this->range_borders_current) {
        this->insert_element(this, new_element);
        return 0;
    } else { // insertion happens somewhere "in the middle"
        this->insert_element(this, this->range_borders[this->range_borders_current - 1]);
        for (int i = this->range_borders_current - 2; i >= index; --i){
            this->range_borders[i + 1] = this->range_borders[i];
        }
        this->range_borders[index] = new_element;
    }
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
    this->range_borders[--this->range_borders_current] = NULL; // TODO: free memory

    // check if we can resize the array to half
    if (this->range_borders_current < this->range_borders_max / 2) {
        this->range_borders_max /= 2;
        struct Delimiter* tmp = (Delimiter*)realloc(this->range_borders, (this->range_borders_max) * sizeof(Delimiter));
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
    
    // check if we insert the first rule, if yes we just insert the borders
    if (this->range_borders_current == 0) {
        struct Delimiter* new_begin_entry = malloc(sizeof(Delimiter));
        struct Delimiter* new_end_entry = malloc(sizeof(Delimiter));
        if ((new_begin_entry == NULL) || (new_end_entry == NULL))
            return 1;
        
        new_begin_entry->delimiter_value = begin_index;
        struct Bv_list* begin_list = list_ctor();
        begin_list->append(begin_list, rule_index);
        new_begin_entry->rule_list = begin_list;
        
        new_end_entry->delimiter_value = end_index;
        struct Bv_list* end_list = list_ctor();
        new_end_entry->rule_list = end_list;
        
        Rb_insert_element(this, new_begin_entry);
        Rb_insert_element(this, new_end_entry);
        
        return 0;
    }
    
    // search through range_borders elements where we can insert the new entries
    int position_begin = find_free_position(this, begin_index);
    
    // check if an element already exists at our border
    if (begin_index == this->range_borders[position_begin]->delimiter_value) {
        // append rule_index to entry
        this->range_borders[position_begin]->rule_list.append(this->range_borders[position_begin]->rule_list, rule_index);
    } else {
        // create new entry with begin_index as new element
        struct Delimiter* new_begin_entry = malloc(sizeof(Delimiter));
        if (new_begin_entry == NULL)
            return 1;
        new_begin_entry->delimiter_value = begin_index;
        struct Bv_list* begin_list = list_ctor();
        
        // append all rules from previous entry here
        Bv_list* tmp =  this->range_borders[position_begin]->rule_list->next;
        while (tmp != NULL) {
            begin_list->append(tmp->rule_index);
            tmp = tmp->next;
        }
        
        begin_list->append(begin_list, rule_index);
        new_begin_entry->rule_list = begin_list;
        
        Rb_insert_element_at_index(this, new_begin_entry, position_begin);
    }
    
    // do the same for end_index
    int position_end = find_free_position(this, end_index);
    
    if (end_index == this->range_borders[position_begin]) {
        return 0;
    } else {
        struct Delimiter* new_end_entry = malloc(sizeof(Delimiter));
        if (new_end_entry == NULL)
            return 1;
        new_end_entry->delimiter_value = end_index;
        struct Bv_list* end_list = list_ctor();
        // append all rules from previous entry here
        Bv_list* tmp =  this->range_borders[position_end]->rule_list->next;
        while (tmp != NULL) {
            end_list->append(end_list, tmp->rule_index);
            tmp = tmp->next;
        }
        new_end_entry->rule_list = end_list;
        Rb_insert_element_at_index(this, new_end_entry, position_end);
    }

    return 0;
}

// Binary search, returns index
uint64_t Rb_find_element(struct Range_borders* this, uint32_t value) {
    if (this->range_borders_current == 0)
        return -1;
    return binary_search(this, value, 0, this->range_borders_current - 1);
}

// Match a header field value of an incoming packet
uint64_t Rb_match_packet(struct Range_borders* this, uint32_t header_value) {
    return 0;
}

uint64_t binary_search(struct Range_borders* this, uint32_t value, uint32_t lower, uint32_t upper) {
    if (upper < lower)
        return -1;
    
    int mid_index = lower + ((upper - lower) / 2);
    int current = this->range_borders[mid_index]->delimiter_value;
    if (current > value)
        return binary_search(this, value, lower, mid_index - 1);
    else if (current < value)
        return binary_search(this, value, mid_index + 1, upper);
        
    return mid_index;
}

uint32_t find_free_position(struct Range_borders* this, uint32_t target) {
    int lower = 0;
    int upper = this->range_borders_current - 1;
    while (lower != upper) {
        int mid_index = lower + ((upper - lower) / 2);
        if (this->range_borders[mid_index]->delimiter_value <= target) {
            lower = mid_index + 1;
        } else {
            upper = mid_index;
        }
    }
    return upper;
}
