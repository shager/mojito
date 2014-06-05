#ifndef BV_TYPES_H
#define BV_TYPES_H 1

#define INIT_SIZE 4

typedef struct Range_borders_t {
    int range_borders_max; //track what max index the array has at the moment
    int range_borders_current; //track the current first unused element
    int *range_borders;
 
    int (*insert_element)(struct Range_borders_t*, int);
    int (*delete_element)(struct Range_borders_t*, int);
} Range_borders;

struct Range_borders_t* range_borders_ctor();
void range_borders_dtor(struct Range_borders_t* this);
int Rb_insert_element(struct Range_borders_t* this, int new_element);
int Rb_delete_element(struct Range_borders_t* this, int index);

#endif //BV_TYPES_H
