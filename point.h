/* Base graphics library for coursework 
 *
 * Point
 * 
 * Header file only.
 */

#ifndef BG_POINT
#define BG_POINT

#include <stdlib.h>

struct bg_point {
    unsigned int x, y;
};

struct bg_point *
bg_point_make(
        unsigned int x, 
        unsigned int y) {
    struct bg_point * ret=(struct bg_point *)malloc(sizeof(struct bg_point));
    ret->x = x;
    ret->y = y;
    return ret;
}

#endif  /* BG_POINT */
