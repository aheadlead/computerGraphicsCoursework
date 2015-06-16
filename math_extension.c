#include <stdio.h>  /* fprintf stderr */
#include <assert.h>  /* assert */
#include <math.h>

#include "point.h"

/* C(33, 16) = 1166803110
 * 这是 32 位有符号整数能表达的最大的数字。
 */
static int comb_cache[34][35]={{0}};

int bg_math_comb(int n, int k) {
    /*fprintf(stderr, "shabi !!!!!!!!! %d %d\n", n, k);*/
#ifndef NDEBUG
    assert(0 <= n && n <= 33);
    assert(0 <= k && k <= n);
#endif 

    if (comb_cache[n][k] != 0) {
        /* 待计算的值已经被缓存下来了 */
        return comb_cache[n][k];
    } else if (k == 0 || k == n) {
        return comb_cache[n][k] = 1;
    } else {
        comb_cache[n][k] = bg_math_comb(n-1, k-1) + bg_math_comb(n-1, k);
        return comb_cache[n][k];
    }
}

double euclidian_distance(
        struct bg_point from,
        struct bg_point to) {
    int t1=(from.x-to.x);
    t1 *= t1;
    int t2=(from.y-to.y);
    t2 *= t2;
    return sqrt(t1+t2);
}
