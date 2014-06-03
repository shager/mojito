#include <stdlib.h>

#define INIT_SIZE 16

typedef struct Range_borders_t {
    int _range_borders_max;// = INIT_SIZE; //track what max index the array has at the moment
    int _range_borders_current; //= 0; //track the current first unused element
    int _range_borders[INIT_SIZE];
    
    int (* insert_element)(int*, int);
};

struct Range_borders_t range_borders_ctor(int range_borders_max, int range_borders_current) {

}

int Rb_insert_element(struct Range_borders_t* this, int* array, int newElement) {
    if (this->range_borders_current < this->rangeBordersMax) {
        array[rangeBordersCurrent++] = newElement;
    } else {
        rangeBordersMax = 2 * rangeBordersMax;
        array = (int*) realloc(array, (rangeBordersMax) * sizeof(int));
        array[rangeBordersCurrent++] = newElement;
    }
    return 0;
}

int Rb_delete_element(struct Range_borders_t* this, int* array, int index) {
    if (sizeof(array) sizeof(array[0]) > index - 1)
        return 1;
    for (int i = index; i < this->range_borders_current; ++i) {
        array[i] = array[i + 1];
    }
    array[this->range_borders_current--] = 0;
    
    // check if we can resize the array to half
    if (this->range_borders_current < this->range_borders_max / 2) {
        this->range_borders_max /= 2;
        array = (int*) realloc(array, (this->range_borders_max) * sizeof(int));
    }
    return 0;
}
