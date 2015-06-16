/* This file can be included by draw.c only. */
#ifndef ALGO_BEZIERCURVE_INCLUDE_ONCE
#define ALGO_BEZIERCURVE_INCLUDE_ONCE
#else
#error "This file can be included once. (by draw.c)"
#endif

#include <stdio.h>  /* fprintf */
#include <stdlib.h>  /* malloc free */
#include <math.h>  /* pow */

#include "point.h"
#include "point_list.h"
#include "math_extension.h"  

const int beziercurve_point_number=500;
struct bg_point_list *
algo_beziercurve_berzier(struct bg_point_list * plist) {
    /* N阶贝塞尔曲线 */

    /* B(t)=sigma(i, 0, n, C(n, i)*Pi*(1-t)^(n-i)*t^i;
     * B 是贝塞尔曲线（坐标点构成）
     * t 是参数变量
     * n 是点数减去一（请注意，点的编号是从0开始编号的）
     * Pi 是第 i 个点的坐标
     */

    /* n的值 */
    int n=plist->length-1;  
    struct bg_point * P=plist->head_p;  /* 点的列表 */
    
    struct bg_point_list * ret=  /* 返回的点列表 */
        (struct bg_point_list *)malloc(sizeof(struct bg_point_list));
    
    /* 给返回的点的列表分配空间 */
    ret->length = beziercurve_point_number;
    ret->head_p = (struct bg_point *)malloc(
            beziercurve_point_number * sizeof(struct bg_point));
    struct bg_point * B=ret->head_p;

    /* 计算点 */
    for (int point_id=0;  /* 枚举曲线点 */
            point_id<beziercurve_point_number; 
            ++point_id) {
        /* 参数方程中的参数t */
        double t=(double)point_id/beziercurve_point_number;
        /* 目标点的值 */
        double target_x=0.0, target_y=0.0;
        for (int i=0; i<=n; ++i) {  /* sigma */
            int comb=bg_math_comb(n, i);
            double f=comb * pow(1-t, n-i) * pow(t, i);
            target_x += (P[i].x)*f;
            target_y += (P[i].y)*f;
        }
        B[point_id].x = (int)target_x;
        B[point_id].y = (int)target_y;
    }
#ifndef NDEBUG
    /* 输出贝塞尔曲线的每个点 */
    for (int i=0; 
            i < ret->length;
            ++i) {
        printf("(%d, %d) => ", ret->head_p[i].x, ret->head_p[i].y);
    }
    putchar('\n');
#endif  /* NDEBUG */

    return ret;
}
