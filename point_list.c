#include "point.h"
#include "point_list.h"

/* bg_point_list_append()
 * Allocate a new list and it is {sizeof(struct bg_point)} bytes bigger than olds,
 * then copy the content from olds to news. The old list would be freed.
 */
void 
bg_point_list_append(
        struct bg_point_list * point_list_p,
        struct bg_point * new_point_p) {
    /* allocate for new list*/
    unsigned int
        new_head_size=(point_list_p->length+1)*sizeof(struct bg_point);
    struct bg_point * 
        new_head_p=(struct bg_point *)malloc(new_head_size);

    /* copy */
    if (NULL != point_list_p->head_p) {
        memcpy(new_head_p, 
                point_list_p->head_p, 
                sizeof(struct bg_point)*point_list_p->length);
    }

    /* append */
    *(new_head_p + point_list_p->length) = *new_point_p;

    /* update point list size value */
    point_list_p->length += 1;

    /* free old list */
    if (0 < point_list_p->length || NULL != point_list_p->head_p)
        free(point_list_p->head_p);

    /* replace head pointer */
    point_list_p->head_p = new_head_p;

    return;
}
