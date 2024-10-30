#include <stdio.h>
#include <stdlib.h>

int main() 
{
	int n = 0;
	
	printf("원하시는 배열의 크기를 입력해주세요 : ");
	scanf("%d", &n);
	
	int* nums = (int *)malloc(sizeof(int) * n);
	printf("\n\n배열의 값들을 입력해 주세요 : ");
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
	
	printf("정렬한 결과 : ");
	for (i = 0; i < n; i++) printf("%d ", nums[i]);
	
	printf("\n");
	return 0;
}
