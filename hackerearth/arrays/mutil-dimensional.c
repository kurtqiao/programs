/*
Transpose 2D array nxm
sameple input:
3 5    >>>n-total rows, m-total columns
13 4 8 14 1 
9 6 3 7 21 
5 12 17 9 3
sample output:
13 9 5 
4 6 12 
8 3 17 
14 7 9 
1 21 3
*/
#include <stdio.h>
 
int main()
{   int n, m;
    int i, j;
    int a[10][10];
    //get 2D array A, n- rows, m - columns
    scanf("%d %d", &n, &m);
    //get array data from input n x m
    for (i=0; i<=n-1; i++){
    	for (j=0; j<=m-1; j++){
    		scanf("%d", &a[i][j]);
    	}
    }
    //convert print
    for (i=0; i<=m-1; i++){
    	for (j=0; j<=n-1; j++){
    		printf("%d ", a[j][i]);
    	}
    	printf("\n");
    }
 
    return 0;
}
