#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {

    int n1 = 0, n2 = 0, n3 = 0, n4 = 0, n5 = 0, n6 = 0;
    int dice;
    char c = 0;

    srand((unsigned)time(NULL));
    printf("주사위를 던지세요. 아무키나 누르면 주사위를 던지게 됩니다.: ");
    printf("아무키나 입력하세요.: ");
    scanf_s(" %c", &c);

    while (1)
    {
        if (c == 'x')
            break;
        else
        {   dice = (rand() % 6) + 1;
        if (dice == 1)
            n1++;
        else if (dice == 2)
            n2++;
        else if (dice == 3)
            n3++;
        else if (dice == 4)
            n4++;
        else if (dice == 5)
            n5++;
        else if (dice == 6)
            n6++;
        }
    }
    printf("1(%d번), 2(%d번), 3(%d번), 4(%d번), 5(%d번), 6(%d번)", n1, n2, n3, n4, n5, n6);

    return 0;
}


