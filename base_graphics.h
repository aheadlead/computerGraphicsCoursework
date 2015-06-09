/* Base graphics library for coursework
 *
 * This library can be considered a highly simplfied OpenGL library for
 * particular coursework.
 * It supplys a few primitive and limited features.
 * 
 * Features:
 *  + A main window with 400px(fixed) width and height both
 *  + Right-click menu support but single level only
 *  + Callback function support when left-click mouse and keyboard events
 *  + Monocolor(black & white) only for less code
 *  + Drawing simple figure like line/Bezier curve/rectangle/circle
 *  + Two styles for lines
 *  + Filling a area based on given point
 *  + Putting fixed font text by given position
 *  + Selection support for features below
 *  + Moving selections
 *  + Resizing selections
 *  + Rotating selections
 *  + Clipping lines
 *  + Undo support
 *
 * Notice: Exception would NOT be detected in this library.
 */
#ifndef BASE_GRAPHICS_H
#define BASE_GRAPHICS_H

#include <stddef.h>  /* size_t */

/* functions */
void bg_init();
void bg_mainloop();

/*
void bg_selection_move(
        bg_point * LU_p,
        bg_point * RD_p,
        bg_point * delta);

void bg_selection_rotate(
        bg_point * LU_p,
        bg_point * RD_p,
        double radian);
*/

#endif /* BASE_GRAPHICS_H */

