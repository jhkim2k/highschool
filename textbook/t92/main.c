#include <stdio.h>
#include <stdlib.h>

int main()
{
   char ch;
   printf("영어 소문자를 입력하세요:");
   scanf("%c", &ch);
   switch (ch)
   {
       case 'a': printf("모음\n");break;
       case 'e': printf("모음\n");break;
       case 'i': printf("모음\n");break;
       case 'o': printf("모음\n");break;
       case 'u': printf("모음\n");break;
       default : printf("자음\n");
   }
}
