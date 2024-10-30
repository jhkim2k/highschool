#include <stdio.h>
#include <stdlib.h>

int main()
{
   printf("%d %d \n", 3==2, 3!=2);
   printf("%d %d \n", 3> 2, 3<=2);
   printf("%d \n", (3>2) && (3<=2));
   printf("%d \n", (3>2) ||(3<=2));
   printf("%d \n", !(3>2));
}
