#include <stdio.h>
#include <stdlib.h>

int main()
{
   char ch;
   printf("���� �ҹ��ڸ� �Է��ϼ���:");
   scanf("%c", &ch);
   switch (ch)
   {
       case 'a':
       case 'e':
       case 'i':
       case 'o':
       case 'u': printf("����\n"); break;
       default : printf("����\n");
   }
}
