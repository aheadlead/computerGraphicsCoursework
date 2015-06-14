/* unit test for math_extension.c */
#include <stdio.h>

#include "math_extension.c"

int main() {
    int n, k;
    while (1) {
        scanf("%d%d", &n, &k);
        printf("%d", bg_math_comb(n, k));
        for (int i=0; i<34; ++i) {
            for (int j=0; j<=i; ++j)
                printf("%10d", comb_cache[i][j]);
            putchar('\n');
        }
    }
    return 0;
}
