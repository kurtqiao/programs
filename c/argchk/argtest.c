/**************
* print out all args 
* and check if args including "test" and "check"
*
***************/
#include <stdio.h>
#include <string.h>

int
main(
 int argc,
 char *argv[]
)
{
 int i,j,rc;
 char *chk[]={"test", "check"};

 for (i=1;i<argc;i++){
    printf("arg%d: %s\n", i, argv[i]);
 }
 for (i=0;i<2;i++){
   for (j=1;j<argc;j++){
   rc=strcmp(argv[j],chk[i]);
   if(!rc){
     printf("found %s in argv!\n", chk[i]);
   }
   }
 }
 return 0;
}
