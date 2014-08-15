#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "asm.h"
#include "bv_types.h"

Bitvector* bitvector_ctor() {
    Bitvector* object = NULL;
    object = malloc(sizeof(Bitvector));
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

int Bv_insert_rule_at_position(Bitvector** this, uint32_t position, uint8_t value) {
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
    //uint32_t bitvector_old_length = (*this)->bitvector_length;
    uint32_t bitvector_blocks = 0;
    if ((*this)->bitvector_length > 0) {
        bitvector_blocks = (((*this)->bitvector_length - 1) / stepsize) + 1;
    }
    
    if (position >= (*this)->bitvector_length) {
        (*this)->bitvector_length = position + 1;
        // Check if we need to extend our bitvector by one or more blocks
        if (bitvector_blocks == 0 || bitvector_blocks < (position / stepsize) + 1) {
            (*this)->bitvector = realloc((*this)->bitvector, sizeof(uint64_t) * ((position / stepsize) + 1));
            for (int i = bitvector_blocks; i < ((position / stepsize) + 1); i++) {
                (*this)->bitvector[i] = 0;
            }
        }
    }
    
    // find out which element ("block") in the array has to be altered
    uint32_t block_in_array = position / stepsize;
    
    // the 0th element is the first (i.e. highest) bit in the vector, NOT the smallest!
    uint32_t bit_in_block = position % stepsize;
    bit_in_block = (sizeof(uint64_t) * 8) - (bit_in_block + 1);
    
    // Finally: insert new bit at correct position
    // Replace the bit in the original integer by a 0 by ANDing it with the inverted mask
    (*this)->bitvector[block_in_array] &= ~((uint64_t)((uint64_t)0x1 << (uint64_t)(bit_in_block)));
    // now overwrite the bit with 0 or 1
    (*this)->bitvector[block_in_array] |= (uint64_t)((uint64_t)value << (uint64_t)(bit_in_block)); 
    
    return 0;
}

int Bv_delete_rule_from_position(Bitvector** this, uint32_t position) {
    (*this)->insert_rule_at_position(this, position, 0);
    return 0;
}

// merges Bitvectors first and second into second
int Bv_merge_bitvectors(Bitvector* first, Bitvector** second) {
    assert(first->bitvector_length != 0);
    
    const uint8_t stepsize = (sizeof(uint64_t) * 8);
    
    uint32_t blocks_in_bv1 = ((first->bitvector_length - 1) / stepsize) + 1;
    uint32_t blocks_in_bv2 = 0;
    if ((*second)->bitvector_length != 0) {
        blocks_in_bv2 = (((*second)->bitvector_length - 1) / stepsize) + 1;
    }
    
    if (blocks_in_bv2 < blocks_in_bv1) {
        (*second)->bitvector = realloc((*second)->bitvector, blocks_in_bv1 * sizeof(uint64_t));
    }
    
    for (int i = blocks_in_bv2; i < blocks_in_bv1; i++) {
        (*second)->bitvector[i] = 0;
    }
    
    for (int i = 0; i < blocks_in_bv1; i++) {
        (*second)->bitvector[i] |= first->bitvector[i];
    }
    
    (*second)->bitvector_length = first->bitvector_length;
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
        object->range_borders[i].bitvector = NULL;
    }
    object->range_borders_max = INIT_SIZE;
    object->range_borders_current = 0;
    object->insert_element = Rb_insert_element;
    object->insert_element_at_index = Rb_insert_element_at_index;
    object->delete_element = Rb_delete_element;
    object->add_rule = Rb_add_rule;
    object->find_element = Rb_find_element;
    object->match_packet = Rb_match_packet;

    // This section has to be added in order to create some executable
    // because OpenFlow does lookups at startup
    char* code = NULL;
    uint64_t tmp_array[1] = {0};
    uint32_t size = construct_node(tmp_array, 1, 0, 0, &code);
    void* mem = mmap(NULL, size, PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    memcpy(mem, code, size);
    free(code);
    object->jit_lookup = mem;
    object->jit_lookup_size = size;
    
    return object;
}

void bitvector_dtor(Bitvector* this) {
    free(this->bitvector);
    this->bitvector = NULL;
}

void delimiter_dtor(Delimiter* this) {
    free(this);
    this = NULL;
}

void range_borders_dtor(Range_borders* this) {
    for (int i = 0; i < this->range_borders_current; ++i) {
        bitvector_dtor(this->range_borders[i].bitvector);
    }
    munmap(this->jit_lookup, this->jit_lookup_size);
    delimiter_dtor(this->range_borders);
    free(this);
    this = NULL;
}

// Append one element to the array
int Rb_insert_element(Range_borders** this, Delimiter* new_element) {
    if ((*this)->range_borders_current < (*this)->range_borders_max) {
        (*this)->range_borders[(*this)->range_borders_current++] = *new_element;
    } else {
        (*this)->range_borders_max *= 2;
        Delimiter* tmp = realloc((*this)->range_borders, ((*this)->range_borders_max) * sizeof(Delimiter));
        if (tmp != NULL) {
            (*this)->range_borders = tmp;
        } else {
            free(tmp);
            return 1;
        }
        (*this)->range_borders[(*this)->range_borders_current++] = *new_element;
    }
    return 0;
}

// Inserts an element at specified index. If index is greater or equal 
// range_borders_current, the new_element is inserted in the last position of the array
int Rb_insert_element_at_index(Range_borders** this, Delimiter* new_element, uint32_t index) {
    // check if the insertion happens in a new slot at the end of the array
    if (index >= (*this)->range_borders_current) {
        (*this)->insert_element(this, new_element);
        return 0;
    } else { // insertion happens somewhere "in the middle"
        Delimiter* tmp_delim = malloc(sizeof(Delimiter));
        memcpy(tmp_delim, &((*this)->range_borders[(*this)->range_borders_current - 1]), sizeof(Delimiter));
        (*this)->insert_element(this, tmp_delim);
        for (int64_t i = (*this)->range_borders_current - 2; i >= index; --i) {
            (*this)->range_borders[i + 1] = (*this)->range_borders[i];
        }
        (*this)->range_borders[index] = *new_element;
        free(tmp_delim);
    }
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
        Delimiter* tmp = realloc(this->range_borders, (this->range_borders_max) * sizeof(Delimiter));
        if (tmp != NULL) {
            this->range_borders = tmp;
        } else {
            return 1;
        }
    }
    return 0;
}

// Insert a new rule into the routing table
int Rb_add_rule(Range_borders* this, uint64_t begin_index, uint64_t end_index, uint32_t rule_index) {
    int position_begin = find_free_position(this, begin_index);
    
    for (int i = 0; i < position_begin; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(&(this->range_borders[i].bitvector), rule_index, 0);
    }
    
    // insert new range_border entry, if it doesn't exist
    if (this->range_borders[position_begin].bitvector != NULL //check edgecase: first insertion
        && begin_index == this->range_borders[position_begin].delimiter_value
        && this->range_borders[position_begin].bitvector->bitvector_length != 0) {
        this->range_borders[position_begin].bitvector->insert_rule_at_position(&(this->range_borders[position_begin].bitvector), rule_index, 1);
    } else {
        Delimiter* new_begin_entry = NULL;
        new_begin_entry = malloc(sizeof(Delimiter));
        if (new_begin_entry == NULL)
            return 1;
        new_begin_entry->delimiter_value = begin_index;
        
        Bitvector* begin_bitvector = bitvector_ctor();
        begin_bitvector->insert_rule_at_position(&begin_bitvector, rule_index, 1);
        
        // append all rules from previous entry here
        if (position_begin != 0) {
            begin_bitvector->merge_bitvectors(this->range_borders[position_begin - 1].bitvector, &begin_bitvector);
        }
        
        new_begin_entry->bitvector = begin_bitvector;
        this->insert_element_at_index(&this, new_begin_entry, position_begin);
        
        free(new_begin_entry);
        new_begin_entry = NULL;
    }
    
    // determine end of insertion area
    int position_end = find_free_position(this, end_index);
    
    for (int i = position_begin + 1; i < position_end; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(&(this->range_borders[i].bitvector), rule_index, 1);
    }
    
    //insert new range_border entry, if it does not exist
    if (position_end < this->range_borders_current && end_index == this->range_borders[position_end].delimiter_value) {
        this->range_borders[position_end].bitvector->insert_rule_at_position(&(this->range_borders[position_end].bitvector), rule_index, 0);
    } else {
        Delimiter* new_end_entry = NULL;
        new_end_entry = malloc(sizeof(Delimiter));
        if (new_end_entry == NULL)
            return 1;
        new_end_entry->delimiter_value = end_index;
        
        Bitvector* end_bitvector = bitvector_ctor();
        //end_bitvector->insert_rule_at_position(end_bitvector, rule_index, 0);
        
        // append all rules from previous entry here
        end_bitvector->merge_bitvectors(this->range_borders[position_end - 1].bitvector, &end_bitvector);
        
        end_bitvector->insert_rule_at_position(&end_bitvector, rule_index, 0);
        
        new_end_entry->bitvector = end_bitvector;
        this->insert_element_at_index(&this, new_end_entry, position_end);
        
        free(new_end_entry);
        new_end_entry = NULL;
    }
    
    for (int i = position_end + 1; i < this->range_borders_current; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(&(this->range_borders[i].bitvector), rule_index, 0);
    }
    return 0;
}

// Insert a new rule into the JIT routing table
int Rb_add_rule_jit(Range_borders* this, uint64_t begin_index, uint64_t end_index, uint32_t rule_index) {
    int position_begin = find_free_position(this, begin_index);
    
    for (int i = 0; i < position_begin; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(&(this->range_borders[i].bitvector), rule_index, 0);
    }
    
    // insert new range_border entry, if it doesn't exist
    if (this->range_borders[position_begin].bitvector != NULL //check edgecase: first insertion
        && begin_index == this->range_borders[position_begin].delimiter_value
        && this->range_borders[position_begin].bitvector->bitvector_length != 0) {
        this->range_borders[position_begin].bitvector->insert_rule_at_position(&(this->range_borders[position_begin].bitvector), rule_index, 1);
    } else {
        Delimiter* new_begin_entry = NULL;
        new_begin_entry = malloc(sizeof(Delimiter));
        if (new_begin_entry == NULL)
            return 1;
        new_begin_entry->delimiter_value = begin_index;
        
        Bitvector* begin_bitvector = bitvector_ctor();
        begin_bitvector->insert_rule_at_position(&begin_bitvector, rule_index, 1);
        
        // append all rules from previous entry here
        if (position_begin != 0) {
            begin_bitvector->merge_bitvectors(this->range_borders[position_begin - 1].bitvector, &begin_bitvector);
        }
        
        new_begin_entry->bitvector = begin_bitvector;
        this->insert_element_at_index(&this, new_begin_entry, position_begin);
        
        free(new_begin_entry);
        new_begin_entry = NULL;
    }
    
    // determine end of insertion area
    int position_end = find_free_position(this, end_index);
    
    for (int i = position_begin + 1; i < position_end; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(&(this->range_borders[i].bitvector), rule_index, 1);
    }
    
    //insert new range_border entry, if it does not exist
    if (position_end < this->range_borders_current && end_index == this->range_borders[position_end].delimiter_value) {
        this->range_borders[position_end].bitvector->insert_rule_at_position(&(this->range_borders[position_end].bitvector), rule_index, 0);
    } else {
        Delimiter* new_end_entry = NULL;
        new_end_entry = malloc(sizeof(Delimiter));
        if (new_end_entry == NULL)
            return 1;
        new_end_entry->delimiter_value = end_index;
        
        Bitvector* end_bitvector = NULL;
        end_bitvector = bitvector_ctor();
        
        // append all rules from previous entry here
        end_bitvector->merge_bitvectors(this->range_borders[position_end - 1].bitvector, &end_bitvector);
        
        end_bitvector->insert_rule_at_position(&end_bitvector, rule_index, 0);
        
        new_end_entry->bitvector = end_bitvector;
        this->insert_element_at_index(&this, new_end_entry, position_end);

        free(new_end_entry);
        new_end_entry = NULL;
    }
    
    for (int i = position_end + 1; i < this->range_borders_current; ++i) {
        this->range_borders[i].bitvector->insert_rule_at_position(&(this->range_borders[i].bitvector), rule_index, 0);
    }
    
    /*
     * Build JIT code:
     * Get the delimiter_values in an array and then construct JIT
     */
    uint64_t* delim_array = NULL;
    delim_array = (uint64_t*)malloc((this->range_borders_current) * sizeof(uint64_t));
    for (int i = 0; i < this->range_borders_current; i++) {
        delim_array[i] = this->range_borders[i].delimiter_value;
    }
    
    // Unmap (free) the old lookup JIT
    munmap(this->jit_lookup, this->jit_lookup_size);
    
    char* code = NULL;
    uint32_t size = construct_node(delim_array, this->range_borders_current, 0, this->range_borders_current - 1, &code);
    void* mem = mmap(NULL, size, PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    memcpy(mem, code, size);
    free(code);
    free(delim_array);
    this->jit_lookup = mem;
    this->jit_lookup_size = size;
    return 0;
}

// Match a header field value of an incoming packet
uint8_t Rb_match_packet(Range_borders* this, Bitvector** result, const uint64_t header_value) {
    const int relevant_border = find_free_position(this, header_value);
    /* range_borders[relevant_border] either points to
     * a) a value where delimiter_value equals header_value
     * b) the entry right of this value
     */ 
    
    // in case of a smaller header value than the smallest delimiter_value
    if (relevant_border == 0 && this->range_borders[0].delimiter_value != header_value) {
        *result = bitvector_ctor();
        return 1;
    }
    
    // in case of an exact hit on a delmiter_value return that (a)
    if (header_value == this->range_borders[relevant_border].delimiter_value) {
        *result = this->range_borders[relevant_border].bitvector;
        return 0;
    }
    
    // case (b)
    *result = this->range_borders[relevant_border - 1].bitvector;
    return 0;
}

// JIT search
uint8_t Rb_match_packet_jit(Range_borders* this, Bitvector** result, const uint64_t header_value) {
    const int relevant_border = this->jit_lookup(header_value);
    
    //filter out case where 0th element is returned, but header_value is smaller than that element
    if ((relevant_border == 0) && ((this->range_borders[0].delimiter_value > header_value) || (this->range_borders[0].bitvector == NULL))) {
        *result = bitvector_ctor();
        return 1;
    }
    *result = this->range_borders[relevant_border].bitvector;
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
