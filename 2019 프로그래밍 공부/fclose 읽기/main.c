#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *fp1=fopen("out.text", "w");
    fprintf(fp1, "&d\n", 9);
    fclose(fp1);
    return 0;
}
