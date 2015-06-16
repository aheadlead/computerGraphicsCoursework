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
#include "algo_line.h"
#include "algo_beziercurve.h"

/* public */

/* ROTATE_R(x) 循环右移
 * x 是一个整型变量。
 */
#define ROTATE_R(x) \
    (x) = ((x&0x1) << (8*sizeof(x)-1))|((unsigned)x>>1);
static int line_pattern=0xFFFFFFFF;
void bg_draw_line_set_pattern(int pattern) {
    line_pattern = pattern;
    return;
}

void bg_draw_line(
        struct bg_point * from_p, 
        struct bg_point * to_p) {
#define DEBUG_BG_DRAW_LINE
#ifdef DEBUG_BG_DRAW_LINE
    printf("draw line from (%d, %d) to (%d, %d)\n", 
            from_p->x, from_p->y,
            to_p->x, to_p->y); 
#endif  /* DEBUG_BG_DRAW_LINE */ 
#ifndef NDEBUG 
    assert(0 <= from_p->x && from_p->x < 400);
    assert(0 <= from_p->y && from_p->y < 400);
    assert(0 <= to_p->x && to_p->x < 400);
    assert(0 <= to_p->y && to_p->y < 400);
#endif  /* NDEBUG */

    struct bg_point_list * point_list_p=
        algo_line(
                from_p->x, from_p->y,
                to_p->x, to_p->y);

    for (struct bg_point * p=point_list_p->head_p;
            p < point_list_p->head_p+point_list_p->length;
            ++p) {
        if (0 <= p->x && p->x < 400 && 0 <= p->y && p->y < 400) {
            current[p->x][p->y] = line_pattern & 0x1;
            ROTATE_R(line_pattern);
        }
    }

    /* free point_list_p */
    if (NULL != point_list_p->head_p) {
        free(point_list_p->head_p);
        point_list_p->head_p = NULL;
    }

    return;
}

void bg_draw_polyline(
        struct bg_point_list * point_list_p, 
        struct bg_point * last_point_p) {
    
    struct bg_point * arr=point_list_p->head_p;
    size_t length=point_list_p->length; 

    if (NULL != last_point_p) {
        bg_draw_line(arr+length-1, last_point_p); 
    }

    for (int i=1; i<length; ++i) {
        bg_draw_line(arr+i-1, arr+i);
    }


    return;
}

void bg_draw_beziercurve(
        struct bg_point_list * point_list_p,
        struct bg_point * last_point_p) {
    /* 将要待计算的控制点放到新的一块内存进行 
     * 如果有 last_point_p 不为 NULL 的话
     * 把 last_point 接上去
     */
    size_t new_point_list_size= 
        (point_list_p->length+(NULL != last_point_p?1:0)) * sizeof(struct bg_point);
    struct bg_point_list * plist=
        (struct bg_point_list *)malloc(sizeof(struct bg_point_list));
    plist->head_p = malloc(new_point_list_size);
    plist->length = (point_list_p->length+(NULL != last_point_p?1:0));  /* 之前漏掉了这一句，痛苦。 */

    /* copy */
    memcpy( plist->head_p,
            point_list_p->head_p, 
            point_list_p->length*sizeof(struct bg_point));

    /* append if last_point_p is not null pointer */
    if (NULL != last_point_p) {
        *(plist->head_p + point_list_p->length) = *last_point_p;
    }

    /* 将控制点传入算法函数 */
    struct bg_point_list * to_draw_list_p=
        algo_beziercurve_berzier(plist);

    /* 画图 */
    /* 实际上调用画折线的函数 */
    bg_draw_polyline(to_draw_list_p, NULL);

    free(to_draw_list_p); to_draw_list_p = NULL;

    free(plist); plist = NULL;
    return;
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
        free(f); f = NULL;
        list_del(head.next);  /* delete the first element in queue */
        free(fn); fn = NULL;
    }
    return;
}

void bg_draw_flush() { 
    glutPostRedisplay(); 
    return;
}

