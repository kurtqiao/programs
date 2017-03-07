/*
print array in reverse
*/
#include <stdio.h>
 
int main()
{
	int n=0;
	int i=0;
	int arr_dt[100];
	scanf("%d", &n);
	for(i=0; i <= n-1; i++){
		scanf("%d", &arr_dt[i]);
	}
	for (i=n-1; i>=0; i--){
		printf("%d\n", arr_dt[i]);
	}
    return 0;
}
