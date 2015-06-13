/* Base graphics library for coursework 
 *
 * Drawing
 */

#include <stdio.h>  /* fprintf(for debugging) */
#include <assert.h>  /* assert */

#include <glut/GLUT.h>
#include "linux/list.h"

/* Though the OpenGL APIs used in this program is abandoned and outdated, I have 
 * to use it still because I don't have plenty of time to improve my coursework and 
 * use new version APIs.
 */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "point.h"
#include "point_list.h"
#include "frame_cache.h"
extern bg_frame 
current;

/* algorithm */

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

        x += 1;
        e = e+2*dy;
        if (e > 0) {
            y += 1;
            e = e-2*dx;
        }
    }
    return ret;
}

struct bg_point_list * algo_line(int x0, int y0, int x1, int y1)
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
    assert(part_number < 8 && part_number >= 0);
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
    /* TODO debugging */
    fprintf(stderr, "pn : %d\n", pn);
    fprintf(stderr, "mapped coodinate : %d, %d\n", mapped_x1, mapped_y1);
    fprintf(stderr, "dx, dy : %d, %d\n", dx, dy);
    /* end of debugging */
    /* 求直线绘制的的每个点相对于 (x0, y0) 的增量 */
    ret = algo_line_bresenham(0, 0, mapped_x1, mapped_y1);

    /* TODO debugging: output ret */
    fprintf(stderr, "###drawing point sequence start\n");
    for (struct bg_point * p=ret->head_p;
            p < ret->head_p+ret->length;
            ++p) {
        fprintf(stderr, "x=%d\ty=%d\n", p->x, p->y);
    }
    fprintf(stderr, "###drawing point sequence end\n\n");
    /* end of debugging */

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

/* public */

void bg_draw_line(
        struct bg_point * from_p, 
        struct bg_point * to_p) {

    struct bg_point_list * point_list_p=
        algo_line(
                from_p->x, from_p->y,
                to_p->x, to_p->y);

    for (struct bg_point * p=point_list_p->head_p;
            p < point_list_p->head_p+point_list_p->length;
            ++p) {
        if (0 <= p->x && p->x < 400 && 0 <= p->y && p->y < 400) {
            current[p->x][p->y] = 1;
        }
    }

    /* free point_list_p */
    if (NULL != point_list_p->head_p) {
        free(point_list_p->head_p);
    }

    return;
}

void bg_draw_curve(
        struct bg_point_list * point_list_p) {
}

void bg_draw_rectangle(
        struct bg_point * LU_p, 
        struct bg_point * RD_p) {
    struct bg_point LD={.x=LU_p->x, .y=RD_p->y};
    struct bg_point RU={.x=RD_p->x, .y=LU_p->y};
    bg_draw_line(LU_p, &RU);
    bg_draw_line(&RU, RD_p);
    bg_draw_line(RD_p, &LD);
    bg_draw_line(&LD, LU_p);

    return;
}

void bg_draw_circle(
        struct bg_point * center_p, 
        double radius) {

}

struct __point_list_node {
    struct bg_point * point_p;
    struct list_head list;
};

struct __point_list_node *
__make_node(
        struct bg_point * point_p) {
    struct __point_list_node * ret;
    ret = (struct __point_list_node *)malloc(sizeof(struct __point_list_node));
    ret->point_p = point_p;
    return ret;
}

void bg_draw_fill(struct bg_point * position_p) { 
    struct list_head head;

    INIT_LIST_HEAD(&head);


    list_add_tail(
            &__make_node(bg_point_make(
                    position_p->x, 
                    position_p->y)) -> list,  
            &head);
    
    current[position_p->x][position_p->y] = 1;
    
    while (!list_empty(&head)) {
        struct __point_list_node * fn=  /* fn means the pointer of the first node */
            list_first_entry(&head, struct __point_list_node, list);
        struct bg_point * f=      /* f means the pointer of the first element */
            fn->point_p;
        if (f->x+1 < 400 && current[f->x+1][f->y] == 0) {  /* RIGHT */
            current[f->x+1][f->y] = 1;
            list_add_tail(
                    &__make_node(bg_point_make(
                            f->x+1, 
                            f->y)) -> list, 
                    &head);
        }
        if (f->x-1 >= 0 && current[f->x-1][f->y] == 0) {  /* LEFT */
            current[f->x-1][f->y] = 1;
            list_add_tail(
                    &__make_node(bg_point_make(
                            f->x-1,
                            f->y)) -> list, 
                    &head);
        }
        if (f->y+1 < 400 && current[f->x][f->y+1] == 0) {  /* UP */
            current[f->x][f->y+1] = 1;
            list_add_tail(
                    &__make_node(bg_point_make(
                            f->x,
                            f->y+1)) -> list, 
                    &head);
        }
        if (f->y-1 >= 0 && current[f->x][f->y-1] == 0) {  /* DOWN */
            current[f->x][f->y-1] = 1;
            list_add_tail(
                    &__make_node(bg_point_make(
                            f->x,
                            f->y-1)) -> list,
                    &head);
        }
        free(f);
        f = NULL;
        list_del(head.next);  /* delete the first element in queue */
        free(fn);
    }
    return;
}

void bg_draw_flush() { 
    glutPostRedisplay(); 
    return;
}

