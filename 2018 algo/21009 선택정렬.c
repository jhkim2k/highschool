#include <stdio.h>
#include <stdlib.h>

int main() 
{
	int n = 0;
	
	printf("���Ͻô� �迭�� ũ�⸦ �Է����ּ��� : ");
	scanf("%d", &n);
	
	int* nums = (int *)malloc(sizeof(int) * n);
	printf("\n\n�迭�� ������ �Է��� �ּ��� : ");
	int i;
	
	for (i = 0; i < n; i++) scanf("%d", &nums[i]);
	
	int j, min;
	
	for (i = 0; i < n; i++) 
	{
		min = i;
		for (j = i; j < n; j++) if (nums[min] > nums[j]) min = j;
		int tmp = nums[min];
		nums[min] = nums[i];
		nums[i] = tmp;
	}
	
	printf("������ ��� : ");
	for (i = 0; i < n; i++) printf("%d ", nums[i]);
	
	printf("\n");
	return 0;
}
