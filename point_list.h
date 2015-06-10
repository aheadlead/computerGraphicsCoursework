/* Base graphics library for coursework 
 *
 * Fixed Length Point List
 * 
 * Header file only.
 */

#ifndef BG_POINT_LIST
#define BG_POINT_LIST

#include <stdlib.h>  /* malloc */
#include <string.h>  /* memcpy */

#include "point.h"

/* bg_point_list */
struct bg_point_list {
    struct bg_point * head_p;
    size_t length;  /* the number of elements */
};

/* bg_point_list_append()
 * Allocate a new list and it is {sizeof(struct bg_point)} bytes bigger than olds,
 * then copy the content from olds to news. The old list would be freed.
 */
void 
bg_point_list_append(
        struct bg_point_list * point_list_p,
        struct bg_point * new_point_p);

#endif  /* BG_POINT_LIST */
