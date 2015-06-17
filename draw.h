/* Base graphics library for coursework 
 *
 * Drawing
 */

#ifndef BG_DRAW
#define BG_DRAW

#include "point.h"
#include "point_list.h"

/* 样式列表
 *  |01234567890123456789012345678901|
 *  |--------------------------------|    BG_LINE_PATTERN_0
 *  |----------------                |    BG_LINE_PATTERN_1
 *  |--------        --------        |    BG_LINE_PATTERN_2
 *  |----    ----    ----    ----    |    BG_LINE_PATTERN_3
 *  |--  --  --  --  --  --  --  --  |    BG_LINE_PATTERN_4
 *  |- - - - - - - - - - - - - - - - |    BG_LINE_PATTERN_5
 *  
 *  |01234567890123456789012345678901|
 *  |--  --------------------------  |    BG_LINE_PATTERN_6
 *  |----------------------------    |    BG_LINE_PATTERN_7
 */
#define BG_LINE_PATTERN_0   0xFFFFFFFF
#define BG_LINE_PATTERN_1   0xFFFF0000
#define BG_LINE_PATTERN_2   0xFF00FF00
#define BG_LINE_PATTERN_3   0xF0F0F0F0
#define BG_LINE_PATTERN_4   0xCCCCCCCC
#define BG_LINE_PATTERN_5   0xAAAAAAAA
#define BG_LINE_PATTERN_6   0xCFFFFFFC
#define BG_LINE_PATTERN_7   0xFFFFFFF0
void bg_draw_line_set_pattern(int pattern);
void bg_draw_line(
        struct bg_point * from_p, 
        struct bg_point * to_p);

/* It will draw a additional point at the end of point_list_p, if the pointer
 * last_point_p is not NULL.
 */
void bg_draw_polyline(
        struct bg_point_list * point_list_p,
        struct bg_point * last_point_p);

/* It will draw a additional point at the end of point_list_p, if the pointer
 * last_point_p is not NULL.
 */
void bg_draw_beziercurve(
        struct bg_point_list * point_list_p,
        struct bg_point * last_point_p);

void bg_draw_rectangle(
        struct bg_point * LU_p, 
        struct bg_point * RD_p);

void bg_draw_circle(
        struct bg_point * center_p, 
        double radius);

void bg_draw_fill(
        struct bg_point * position_p);

void bg_draw_flush();

void bg_draw_rotate(
        struct bg_point * selection_from,
        struct bg_point * selection_to,
        struct bg_point * vec_from,
        struct bg_point * vec_to);

void bg_draw_transform(
        struct bg_point * selection_from,
        struct bg_point * selection_to,
        struct bg_point * delta);

#endif  /* BG_DRAW */
