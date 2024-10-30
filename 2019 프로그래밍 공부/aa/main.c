#include <stdio.h>

int main()
{
	int j;
	char *g;

	printf("입력: ");
	scanf_s("%d", &j);

	if(j >= 90)
		g = "A";
	else if(j >= 80)
		g = "B";
	else if(j >= 70)
		g = "C";
	else if(j >= 60)
		g = "D";
	else
		g = "F";

	printf("등급: %s\n", g);
	return 0;
}
