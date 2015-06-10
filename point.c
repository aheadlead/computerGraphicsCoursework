/* Base graphics library for coursework 
 *
 * Point
 */

#include <stdlib.h>  /* malloc */

#include "point.h"

struct bg_point *
bg_point_make(
        int x, 
        int y) {
    struct bg_point * ret=(struct bg_point *)malloc(sizeof(struct bg_point));
    ret->x = x;
    ret->y = y;
    return ret;
}
