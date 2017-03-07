/*

input a 1-D array, and remove same value in the array

output 1-D array after strip



*/

#include <stdio.h>
#include <stdlib.h>


int main(){

  int *array;
  int siz;
  int i;
  int *tmp_sptr, *tmp_eptr, *tmp_arrptr;

  printf("input array size:\n");
  scanf("%d", &siz);
  printf("input %d data:\n", siz);

  array = malloc(sizeof(int)*siz);
  for (i = 0; i < siz; i++){
    scanf("%d",&array[i]);
  }

  //strip array

  //compare the array and array+1, if equal, insert the end value in array+1

  //then move array and start for next value

  //sample process:  1 2 1 1 2

  //                 1 2 2 1 x

  //                 1 2 2 x

  //                 1 2 x
  tmp_arrptr = array;
  tmp_eptr = array + siz - 1;
  for (tmp_sptr = array+1; tmp_arrptr < tmp_eptr; tmp_arrptr++, tmp_sptr=tmp_arrptr+1)
  {
    while( tmp_sptr <= tmp_eptr){
      if(*tmp_sptr == *tmp_arrptr){

        //if equal value found, insert the end value here
        *tmp_sptr = *tmp_eptr;

        *tmp_eptr-- = '\0';
      }else{
        tmp_sptr++;
      }

      //DEBUG print

     for(i=0; i<siz; i++){

      printf(" %d ", array[i]);

     }

     printf("\n");

     //DEBUG print
    }
  }



  //*(++tmp_eptr) = '\0';

  printf("out data:\n");
  for (i=0; i<= siz; i++){

    if (array[i] == '\0')

       break;
    printf(" %d ", array[i]);
    }

  free(array);
  return 0;
}
