/* Base graphics library for coursework 
 *
 * Point
 */

#ifndef BG_POINT
#define BG_POINT

#include <stdlib.h>

struct bg_point {
    int x, y;
};

struct bg_point *
bg_point_make(
        int x, 
        int y);

#endif  /* BG_POINT */
