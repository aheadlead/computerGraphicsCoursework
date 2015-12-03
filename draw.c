/* Base graphics library for coursework 
 *
 * Drawing
 */

#include <stdio.h>  /* fprintf(for debugging) */
#include <assert.h>  /* assert */

#include <GLUT/glut.h>
#include "linux/list.h"

/* Though the OpenGL APIs used in this program is abandoned and outdated, I have 
 * to use it still because I don't have plenty of time to improve my coursework and 
 * use new version APIs.
 */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "point.h"
#include "point_list.h"
#include "frame_cache.h"
#define IN_SCREEN(x, y) \
    (0 <= (x) && (x) < 400 && \
     0 <= (y) && (y) < 400)
extern bg_frame 
current;

/* algorithm */
#include "algo_line.h"
#include "algo_beziercurve.h"
struct bg_point_list * 
algo_draw_circle(
        struct bg_point * center_p,
        double radius) {
    struct bg_point_list * ret=
        (struct bg_point_list *)malloc(sizeof(struct bg_point_list));
    ret->head_p = NULL;
    ret->length = 0;

    /* 从教材113页复制 */
    int r=(int)radius;
    int x, y, d;
    x=0; y=r; d=1-r;
    while (x <= y) {
    
        /* 教材里面的 CirclePoint 函数 */
        struct bg_point tmp;
        bg_point_list_append(ret, (tmp.x=x, tmp.y=y, &tmp));
        bg_point_list_append(ret, (tmp.x=y, tmp.y=x, &tmp));
        bg_point_list_append(ret, (tmp.x=-y, tmp.y=x, &tmp));
        bg_point_list_append(ret, (tmp.x=-x, tmp.y=y, &tmp));
        bg_point_list_append(ret, (tmp.x=-x, tmp.y=-y, &tmp));
        bg_point_list_append(ret, (tmp.x=-y, tmp.y=-x, &tmp));
        bg_point_list_append(ret, (tmp.x=y, tmp.y=-x, &tmp));
        bg_point_list_append(ret, (tmp.x=x, tmp.y=-y, &tmp));

        if (d < 0) {
            d += 2*x + 3;
        } else {
            d += 2*(x-y)+5;
            y -= 1;
        }
        x += 1;
    }

    /* 将圆心平移到 center_p 所指向的点 */
    for (struct bg_point * p=ret->head_p;
            p<ret->head_p+ret->length;
            ++p) {
        p->x += center_p->x;
        p->y += center_p->y;
    }

    return ret;
}

struct bg_point_list *
algo_draw_rotate(
        struct bg_point * selection_from,
        struct bg_point * selection_to,
        struct bg_point * vec_from,
        struct bg_point * vec_to) {
    /* 准备点 */
    struct bg_point A, B, C;
    A.x = (selection_from->x + selection_to->x)/2;
    A.y = (selection_from->y + selection_to->y)/2;
    B = *vec_from;
    C = *vec_to;
    double a=euclidian_distance(B, C);
    double b=euclidian_distance(A, C);
    double c=euclidian_distance(A, B);
    /* 余弦定理 */
    double cosA=(b*b+c*c-a*a)/(2*b*c);
    double sinA=sqrt(1-cosA*cosA);

    /* 准备返回的点列表 */
    struct bg_point_list * ret_p;
    ret_p = (struct bg_point_list *)malloc(sizeof(struct bg_point_list));
    ret_p->head_p = NULL;
    ret_p->length = 0;
    
    /* 对每个点做变换 */
    /* 迭代 */
    for (int x=selection_from->x; x<=selection_to->x; ++x){
        for (int y=selection_from->y; y<=selection_to->y; ++y) {
            if (current[x][y] > 0) {  /* 对黑色点进行处理 */
                struct bg_point new;
                x -= A.x, y -= A.y;
                new.x = A.x + (int)(x*cosA-y*sinA);  /* 旋转的那个矩阵 */
                new.y = A.y + (int)(x*sinA+y*cosA);
                x += A.x, y += A.y;
                bg_point_list_append(ret_p, &new);
            }
        }
    }

    return ret_p;
}


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
#ifdef DEBUG_BG_DRAW_LINE
    printf("draw line from (%d, %d) to (%d, %d)\n", 
            from_p->x, from_p->y,
            to_p->x, to_p->y); 
#endif  /* DEBUG_BG_DRAW_LINE */ 
#ifndef NDEBUG 
#ifdef CHECK_LINE_OUT_OF_BOUND
    assert(0 <= from_p->x && from_p->x < 400);
    assert(0 <= from_p->y && from_p->y < 400);
    assert(0 <= to_p->x && to_p->x < 400);
    assert(0 <= to_p->y && to_p->y < 400);
#endif  /* CHECK_LINE_OUT_OF_BOUND */
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
    /* 待画点列表 */
    struct bg_point_list * to_draw_list_p=
        algo_draw_circle(center_p, radius);
    /* alias */
    const struct bg_point * P=to_draw_list_p->head_p;
    const size_t Pn=to_draw_list_p->length;
    /* draw */
    for (const struct bg_point *p=P; p<P+Pn; ++p) {
        if (!(0 <= p->x && p->x < 400)) { continue; }
        if (!(0 <= p->y && p->y < 400)) { continue; }
        current[p->x][p->y] = 1;
    }

    free(to_draw_list_p); to_draw_list_p = NULL;
    return;
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

/* 这个函数将点 selection_from 和 selection_to 所描述的选区，以其几何中心旋转。
 * 旋转角度 A 由点 vec_from 到 vec_to 所组成的向量决定，这两个点与旋转中心构成一
 * 个三角形。向量边所对的角就是要旋转的角。
 */
void bg_draw_rotate(
        struct bg_point * selection_from,
        struct bg_point * selection_to,
        struct bg_point * vec_from,
        struct bg_point * vec_to) {
    struct bg_point_list * to_draw_list=
        algo_draw_rotate(
                selection_from, selection_to,
                vec_from, vec_to);

    /* 迭代以清空选区 */
    for (int x=selection_from->x; x<=selection_to->x; ++x){
        for (int y=selection_from->y; y<=selection_to->y; ++y) {
            current[x][y] = 0;
        }
    }
    
    /* 绘制旋转后的情况 */
    for (struct bg_point * p=to_draw_list->head_p;
            p<to_draw_list->head_p+to_draw_list->length;
            ++p) {
        if (0 <= p->x && p->x < 400 &&
            0 <= p->y && p->y < 400) {
            current[p->x][p->y] = 1;
        }
    } 

    return;
}

/* 点 selection_from 和点 selection_to 描述了一个选区。
 * 这个函数的操作就是把这个选区以 delta 描述的向量，平移。
 */
void bg_draw_transform(
        struct bg_point * selection_from,
        struct bg_point * selection_to,
        struct bg_point * delta) {
    size_t stage_length=
        (selection_to->x - selection_from->x) * (selection_to->y - selection_from->y);
    int * stage=
        (int *)malloc(sizeof(int)*stage_length);
    /* 迭代指针 */
    int * p=stage;

    /* 备份待移动区域 */
    for (int x=selection_from->x; x<selection_to->x; ++x) {
        for (int y=selection_from->y; y<selection_to->y; ++y) {
            *p = current[x][y];
            p += 1;
        }
    }

    /* 将老的区域抹白 */
    for (int x=selection_from->x; x<selection_to->x; ++x) {
        for (int y=selection_from->y; y<selection_to->y; ++y) {
            current[x][y] = 0;
        }
    }

    /* 迭代指针归位 */
    p = stage;

    /* 在新的位置绘制 */
    for (int x=selection_from->x; x<selection_to->x; ++x) {
        for (int y=selection_from->y; y<selection_to->y; ++y) {
            if (IN_SCREEN(x+delta->x, y+delta->y)) {
                current[x+delta->x][y+delta->y] = *p;
            }
            p += 1;
        }
    }

    return;
}
