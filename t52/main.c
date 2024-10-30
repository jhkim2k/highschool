#include <stdio.h>
#include <stdlib.h>

main()
{
    int max=2147483647;
    printf("%l\d", max);
    max=max+1;
    printf("%l\d", max);
}
