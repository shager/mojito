#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "bv_types.h"

Bitvector* bitvector_ctor() {
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

void bitvector_dtor(Bitvector* this) {
    free(this->bitvector);
}

int Bv_insert_rule_at_position(Bitvector* this, uint32_t position, uint8_t value) {
    /* General idea: Identify the block in our bitvector where we want to
     * insert the new bit.
     * 
     * Right-shift all blocks positioned right from the desired block 
     * to make space for the extra bit.
     * 
     * Right-shift the righter bits in our desired block
     * 
     * Insert our new bit
     */
    if (value != 0)
        value = 1;
    
    const uint8_t stepsize = (sizeof(uint64_t) * 8); // stepsize = 64
    uint32_t bitvector_old_length = this->bitvector_length;
    
    if (position >= this->bitvector_length) {
        this->bitvector_length = position + 1;
        // Check if we need to extend our bitvector by one or more blocks
        if ((bitvector_old_length - 1 / stepsize) < (position / stepsize)) { //TODO: check that overflow is no bug here!
            this->bitvector = (uint64_t*)realloc(this->bitvector, sizeof(uint64_t) * ((this->bitvector_length / stepsize) + 1));
        }
    }
    
    // find out which element ("block") in the array has to be altered
    uint32_t block_in_array = position / stepsize;
    
    // the 0th element is the first (i.e. highest) bit in the vector, NOT the smallest!
    uint32_t bit_in_block = position % stepsize;
    bit_in_block = (sizeof(uint64_t) * 8) - (bit_in_block + 1);
    
    // Finally: insert new bit at correct position
    // Replace the bit in the original integer by a 0 by ANDing it with the inverted mask
    this->bitvector[block_in_array] &= ~((uint64_t)((uint64_t)0x1 << (uint64_t)(bit_in_block)));
    // now overwrite the bit with 0 or 1
    this->bitvector[block_in_array] |= (uint64_t)((uint64_t)value << (uint64_t)(bit_in_block)); 
    
    return 0;
}

int Bv_delete_rule_from_position(Bitvector* this, uint32_t position) {
    this->insert_rule_at_position(this, position, 0);
    return 0;
}

// merges Bitvectors first and second into second
//TODO: Test!
int Bv_merge_bitvectors(Bitvector* first, Bitvector* second) {
    const uint8_t stepsize = (sizeof(uint64_t) * 8);
    // check for equal length
    if (second->bitvector_length < first->bitvector_length) {
        second->bitvector = (uint64_t*)realloc(second->bitvector, sizeof(uint64_t) * ((first->bitvector_length / stepsize) + 1));
    }
    
    for (int i = 0; i <= (first->bitvector_length / stepsize); ++i) {
        second->bitvector[i] = second->bitvector[i] | first->bitvector[i];
    }
    return 0;
}

Range_borders* range_borders_ctor() {
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

void range_borders_dtor(Range_borders* this) {
    for (int i = 0; i < this->range_borders_current; ++i) {
        bitvector_dtor(this->range_borders[i].bitvector);
    }
    free(this->range_borders);
    free(this);
}

// Append one element to the array
int Rb_insert_element(Range_borders* this, Delimiter* new_element) {
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
int Rb_insert_element_at_index(Range_borders* this, Delimiter* new_element, uint32_t index) {
    // check if the insertion happens in a new slot at the end of the array
    //printf("Entered insert at index %d...\n", index);
    //fflush(stdout);
    if (index >= this->range_borders_current) {
        this->insert_element(this, new_element);
        return 0;
    } else { // insertion happens somewhere "in the middle"
        this->insert_element(this, &(this->range_borders[this->range_borders_current - 1]));
        for (int64_t i = this->range_borders_current - 2; i >= index; --i){ // was >
            //printf("in loop, i = %d\n", i);
            //fflush(stdout);
            this->range_borders[i + 1] = this->range_borders[i];
        }
        this->range_borders[index] = *new_element;
    }
    //printf("done\n");
    return 0;
}

// Delete a single element in the array
int Rb_delete_element(Range_borders* this, uint32_t index) {
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
int Rb_add_rule(Range_borders* this, uint64_t begin_index, uint64_t end_index, uint32_t rule_index) {
    //TODO: check edgecases (empty array, target = max, rule_index = 0)
    
    int position_begin = find_free_position(this, begin_index);
    //printf("position_begin = %d\n", position_begin);
    
    for (int i = 0; i < position_begin; ++i) {
        //printf("insert 0 for rule %d at position %d\n", rule_index, i);
        this->range_borders[i].bitvector->insert_rule_at_position(this->range_borders[i].bitvector, rule_index, 0);
    }
    
    // insert new range_border entry, if it doesn't exist
    if (begin_index == this->range_borders[position_begin].delimiter_value
        && this->range_borders[position_begin].bitvector->bitvector_length != 0) { //edgecase: first insertion!
        //printf("Reuse old rb entry (position_begin)\n");
        this->range_borders[position_begin].bitvector->insert_rule_at_position(this->range_borders[position_begin].bitvector, rule_index, 1);
    } else {
        //printf("creating new borders entry\n");
        Delimiter* new_begin_entry = malloc(sizeof(Delimiter));
        if (new_begin_entry == NULL)
            return 1;
        new_begin_entry->delimiter_value = begin_index;
        
        Bitvector* begin_bitvector = bitvector_ctor();
        begin_bitvector->insert_rule_at_position(begin_bitvector, rule_index, 1);
        
        // append all rules from previous entry here
        if (position_begin != 0) {
            begin_bitvector->merge_bitvectors(this->range_borders[position_begin - 1].bitvector, begin_bitvector);
        }
        
        new_begin_entry->bitvector = begin_bitvector;
        this->insert_element_at_index(this, new_begin_entry, position_begin);
        
        //indicate point from where we can continue to extend the bitvectors
        position_begin++;
    }
    
    // determine end of insertion area
    int position_end = find_free_position(this, end_index);
    
    for (int i = position_begin; i < position_end; ++i) {
        //printf("insert 1 for rule %d at position %d\n", rule_index, i);
        this->range_borders[i].bitvector->insert_rule_at_position(this->range_borders[i].bitvector, rule_index, 1);
    }
    
    //insert new range_border entry, if it does not exist
    if (end_index == this->range_borders[position_end].delimiter_value) {
        this->range_borders[position_end].bitvector->insert_rule_at_position(this->range_borders[position_end].bitvector, rule_index, 0);
    } else {
        Delimiter* new_end_entry = malloc(sizeof(Delimiter));
        if (new_end_entry == NULL)
            return 1;
        new_end_entry->delimiter_value = end_index;
        
        Bitvector* end_bitvector = bitvector_ctor();
        //end_bitvector->insert_rule_at_position(end_bitvector, rule_index, 0);
        
        // append all rules from previous entry here
        end_bitvector->merge_bitvectors(this->range_borders[position_end - 1].bitvector, end_bitvector);
        
        end_bitvector->insert_rule_at_position(end_bitvector, rule_index, 0);
        
        new_end_entry->bitvector = end_bitvector;
        this->insert_element_at_index(this, new_end_entry, position_end);
        
        //indicate point from where we can continue to extend the bitvectors
        position_end++;
    }
    
    for (int i = position_end; i < this->range_borders_current; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(this->range_borders[i].bitvector, rule_index, 0);
    }
    return 0;
}

//TODO: return value in error case
// Match a header field value of an incoming packet
uint8_t Rb_match_packet(Range_borders* this, Bitvector** result, uint64_t header_value) {
    Bitvector* tmp = bitvector_ctor();
    int relevant_border = find_free_position(this, header_value);
    /* range_borders[relevant_border] either points to
     * a) a value where delimiter_value equals header_value
     * b) the entry right of this value
     */ 
    
    // in case of a smaller header value than the smallest delimiter_value
    if (relevant_border == 0 && this->range_borders[0].delimiter_value != header_value) {
        result = NULL;
        return 1;
    }
    
    // in case of an exact hit on a delmiter_value return that (a)
    if (header_value == this->range_borders[relevant_border].delimiter_value) {
        tmp = this->range_borders[relevant_border].bitvector;
        *result = tmp;
        return 0;
    }
    
    // case (b)
    --relevant_border;
    tmp = this->range_borders[relevant_border].bitvector;
    *result = tmp;
    return 0;
}

// Binary search, returns index
int64_t Rb_find_element(Range_borders* this, uint32_t value) {
    if (this->range_borders_current == 0)
        return -1;
    return binary_search(this, value, 0, this->range_borders_current - 1);
}

int64_t binary_search(Range_borders* this, uint32_t value, uint32_t lower, uint32_t upper) {
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

/* 
 * Returns position to insert in array.
 * target < min(array) -> 0
 * target = min(array) -> 0
 * target > max(array) -> len(array)
 * target = max(array) -> len(array) - 1
 * target element of array -> position of target in array
 * else -> position of first greater element in array
 */
uint32_t find_free_position(Range_borders* this, uint64_t target) {
    int lower = 0;
    int upper = this->range_borders_current;
    if (upper == 0)
        return 0;
    while (lower != upper) {
        int mid_index = lower + ((upper - lower) / 2);
        assert(mid_index != this->range_borders_current);
        if (this->range_borders[mid_index].delimiter_value == target) {
            return mid_index;
        } else if (this->range_borders[mid_index].delimiter_value < target) {
            lower = mid_index + 1;
        } else {
            upper = mid_index;
        }
    }
    return lower;
}
