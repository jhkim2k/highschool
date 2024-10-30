#include <stdio.h>
#include <stdlib.h>

int main()
{
   char ch;
   printf("영어 소문자를 입력하세요:");
   scanf("%c", &ch);
   switch (ch)
   {
       case 'a':
       case 'e':
       case 'i':
       case 'o':
       case 'u': printf("모음\n"); break;
       default : printf("자음\n");
   }
}
