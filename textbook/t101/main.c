#include <stdio.h>
#include <stdlib.h>

int main()
{
    int num;
    scanf("%d", &num);
    while (num > 0);
    {
        if (num % 2 == 1)
            printf("Ȧ��\n");
        else
            printf("¦��\n");
        scanf("%d", &num);
    }
}
