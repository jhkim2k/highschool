#include <stdio.h>
#include <stdlib.h>

int main()
{
    struct add{
        char name[10];
        char id[10];
        int kor, math, eng, total;
        float aver;
    };

    struct add no;
        printf("%d��° �й� : ");
        scanf("%s",no.name);
        printf("%d��° �̸� : ");
        scanf("%s",no.id);
        printf("%d��° ���� : ");
        scanf("%s",no.kor);
        printf("%d��° ���� : ");
        scanf("%s",no.math);
        printf("%d��° ���� : ");
        scanf("%s",no.eng);
    no.total=no.kor + no.math + no.eng;
    no.aver = (float)no.total/3;

        printf("%d��° �й� : %s\n",no.name);
        printf("%d��° �̸� : %s\n",no.id);
        printf("%d��° ���� : %s\n",no.kor);
        printf("%d��° ���� : %s\n",no.math);
        printf("%d��° ���� : %s\n",no.eng);
        printf("%d��° ��� : %s\n",no.aver);
    return 0;
}
