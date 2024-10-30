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
        printf("%d번째 학번 : ");
        scanf("%s",no.name);
        printf("%d번째 이름 : ");
        scanf("%s",no.id);
        printf("%d번째 국어 : ");
        scanf("%s",no.kor);
        printf("%d번째 수학 : ");
        scanf("%s",no.math);
        printf("%d번째 영어 : ");
        scanf("%s",no.eng);
    no.total=no.kor + no.math + no.eng;
    no.aver = (float)no.total/3;

        printf("%d번째 학번 : %s\n",no.name);
        printf("%d번째 이름 : %s\n",no.id);
        printf("%d번째 국어 : %s\n",no.kor);
        printf("%d번째 수학 : %s\n",no.math);
        printf("%d번째 영어 : %s\n",no.eng);
        printf("%d번째 평균 : %s\n",no.aver);
    return 0;
}
