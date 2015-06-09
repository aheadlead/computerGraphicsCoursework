/* Base graphics library for coursework 
 *
 * Point List
 * 
 * Header file only.
 */

#ifndef BG_POINT_LIST
#define BG_POINT_LIST

#include "point.h"

/* bg_point_list */
struct bg_point_list {
    struct bg_point * head_p;
    size_t size;
};

/* bg_point_list_append()
 * Allocate a new list and it is {sizeof(bg_point)} bytes bigger than olds,
 * then copy the content from olds to news. The old list would be freed.
 */
void 
bg_point_list_append(
        bg_point_list * point_list_p,
        bg_point * new_point_p) {
    /* allocate for new list*/
    size_t new_head_size=point_list_p->size + sizeof(bg_point);
    bg_point * new_head_p=(bg_point *)malloc(new_head_size);

    /* copy */
    memcpy(new_head_p, 
            point_list_p->head_p, 
            point_list_p->size);

    /* update point list size value */
    point_list_p->size += sizeof(bg_point);

    /* free old list */
    free(point_list_p->head_p);

    /* replace head pointer */
    point_list_p->head_p = new_head_p;

    return;
}

#endif  /* BG_POINT_LIST */
