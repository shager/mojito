#ifndef BV_TYPES_H
#define BV_TYPES_H 1

#define INIT_SIZE 4

typedef struct Range_borders_t {
    int range_borders_max; //track what max index the array has at the moment
    int range_borders_current; //track the current first unused element
    int range_borders[INIT_SIZE];
 
    int (* insert_element)(struct Range_borders_t*, int);
    int (* delete_element)(struct Range_borders_t*, int);
} Range_borders;

struct Range_borders* range_borders_ctor();
int Rb_insert_element(struct Range_borders_t* this, int new_element);
int Rb_delete_element(struct Range_borders_t* this, int index);

#endif //BV_TYPES_H
