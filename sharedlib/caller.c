#include "mylib.h"
#include <stdio.h>

void main(void) {
    int *size;
    int *ret = mymethod(2, size);

    printf("size is %d\n", *size);
    printf("arr is [%d %d]\n", ret[0], ret[1]);

}
