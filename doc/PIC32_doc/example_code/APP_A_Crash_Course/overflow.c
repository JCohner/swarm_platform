#include <stdio.h>

int main(void) {
  char i = 100, j = 240, sum;
  unsigned char iu = 100, ju = 240, sumu;
  signed char is = 100, js = 240, sums;

  sum = i+j; 
  sumu = iu+ju; 
  sums = is+js;

  printf("char:           %d + %d = %3d or ASCII %c\n",i,j,sum,sum);
  printf("unsigned char:  %d + %d = %3d or ASCII %c\n",iu,ju,sumu,sumu);
  printf("signed char:    %d + %d = %3d or ASCII %c\n",is,js,sums,sums);
  return 0;
}
