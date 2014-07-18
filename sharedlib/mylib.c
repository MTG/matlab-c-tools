#include <stdio.h>
#include <stdlib.h>
int* mymethod(int arg, int *ret1) {
    printf("Hello from mymethod: %d\n", arg);
    int *ans = (int*)malloc(sizeof(int)*2);
    ans[0] = 3;
    ans[1] = 6;
    *ret1 = 2;
    return ans;
}
