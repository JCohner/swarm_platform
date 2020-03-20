#include <stdio.h>

void printRatio(int numer, int denom) {  // printRatio is a helper function
  double ratio;

  ratio = numer/denom;
  printf("Ratio, %1d/%1d:                        %5.2f\n",numer,denom,ratio);
  ratio = numer/((double) denom);
  printf("Ratio, %1d/((double) %1d):             %5.2f\n",numer,denom,ratio);
  ratio = ((double) numer)/((double) denom);
  printf("Ratio, ((double) %1d)/((double) %1d):  %5.2f\n",numer,denom,ratio);
} 

int main(void) {
  int num = 5, den = 2;

  printRatio(num,den);                   // call the helper function
  return(0);
}
