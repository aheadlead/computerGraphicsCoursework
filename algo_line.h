/* This file can be included by draw.c only. */
#ifndef ALGO_LINE_INCLUDE_ONCE
#define ALGO_LINE_INCLUDE_ONCE
#else
#error "This file can be included once. (by draw.c)"
#endif

#include <stdio.h>  /* fprintf */
#include <stdlib.h>  /* malloc free */

#include "point.h"
#include "point_list.h"

struct bg_point_list * 
algo_line_bresenham(
        int x0, int y0, 
        int x1, int y1) {
    struct bg_point_list * ret=(struct bg_point_list *)malloc(
            sizeof(struct bg_point_list));
    ret->head_p=NULL,
    ret->length=0;
    /* Bresenham algorithm */
    int x, y, dx, dy, e;
    dx = x1-x0;
    dy = y1-y0;
    e = -dx;
    x = x0;
    y = y0;
    while (x <= x1) {
        struct bg_point * tmp_point=bg_point_make(x, y);
        bg_point_list_append(ret, tmp_point);
        free(tmp_point);
        tmp_point = NULL;

        x += 1;
        e = e+2*dy;
        if (e > 0) {
            y += 1;
            e = e-2*dx;
        }
    }
    return ret;
}

struct bg_point_list * 
algo_line(int x0, int y0, int x1, int y1)
{
    const int dx=x1-x0,
              dy=y1-y0;
    /* The algorithm on textbook apply to 1/8 panel only.
     * It works when (y1-y0)/(x1-x0) between 0.0 and 1.0, and (y1-y0)>=0 and
     * (x1-x0)>=0.
     *
     * I figured out the problem by a linear transformation.
     */

    /* Divide plane into 8 numbered part. "*" is point 0. 
     *       \ 2|1 /
     *        \ | / 
     *       3 \|/ 0
     *       ---*----
     *       4 /|\ 7
     *        / | \
     *       / 5|6 \
     *      Consider the number of part which the point 1 is in.
     *      妈的实在不会用英语写注释了，这里就是把以点0为坐标原点，看点1落在哪
     *      个 part 里面。
     */
#define ABS(x) ((x)>0?(x):-(x))
    int part_number=0;
    part_number = -2*(dx < 0) + 6*(dy < 0);
    part_number = ABS(part_number);
    switch (part_number) {
        case 0: part_number += (dy > dx); break;
        case 2: part_number += (dy < -dx); break;
        case 4: part_number += (dy < dx); break;
        case 6: part_number += (-dy < dx); break;
        default:
                assert(0);  /* TRAP Part_number is incorrent. */
    }
    /* disabled debugging code */
    /* 
     *if (part_number < 0 || part_number >= 8) {
     *    fprintf(stderr, "我操你大爷 %d\n", part_number);
     *}
     */
#ifndef NDEBUG
    assert(part_number < 8 && part_number >= 0);
#endif  /* NDEBUG */
#undef ABS

    typedef int matrix_2x2[4];
    matrix_2x2 t[8]={
        {1, 0, 0, 1},
        {0, 1, 1, 0},
        {0, -1, 1, 0},
        {-1, 0, 0, 1},
        {-1, 0, 0, -1},
        {0, -1, -1, 0},
        {0, 1, -1, 0},
        {1, 0, 0, -1}
    };
    
    struct bg_point_list * ret;
    const int pn=part_number;  /* alias */
    /* mapping */
    int mapped_x1=dx*t[pn][0] + dy*t[pn][2], 
        mapped_y1=dx*t[pn][1] + dy*t[pn][3];
#ifdef DEBUG_BG_DRAW_LINE
    fprintf(stderr, "pn : %d\n", pn);
    fprintf(stderr, "mapped coodinate : %d, %d\n", mapped_x1, mapped_y1);
    fprintf(stderr, "dx, dy : %d, %d\n", dx, dy);
#endif  /* DEBUG_BG_DRAW_LINE */
    /* 求直线绘制的的每个点相对于 (x0, y0) 的增量 */
    ret = algo_line_bresenham(0, 0, mapped_x1, mapped_y1);

#ifdef DEBUG_BG_DRAW_LINE
    fprintf(stderr, "###drawing point sequence start\n");
    for (struct bg_point * p=ret->head_p;
            p < ret->head_p+ret->length;
            ++p) {
        fprintf(stderr, "x=%d\ty=%d\n", p->x, p->y);
    }
    fprintf(stderr, "###drawing point sequence end\n\n");
#endif  /* DEBUG_BG_DRAW_LINE */

    /* unmapping */
    int u[8]={0, 1, 6, 3, 4, 5, 2, 7};
    for (struct bg_point * p=ret->head_p;
            p < ret->head_p+ret->length;
            ++p) {
        struct bg_point tmp;
        tmp.x = p->x*t[u[pn]][0] + p->y*t[u[pn]][2] + x0;
        tmp.y = p->x*t[u[pn]][1] + p->y*t[u[pn]][3] + y0;
        *p = tmp;
    }

    return ret;
}
