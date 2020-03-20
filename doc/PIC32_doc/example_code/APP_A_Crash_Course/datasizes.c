#include <stdio.h>

int main(void) {
  char a; 
  char *bp; 
  short c; 
  int d; 
  long e; 
  float f; 
  double g; 
  long double h;
  long double *ip; 

  printf("Size of char:                  %2ld bytes\n",sizeof(a)); // "% 2 ell d"
  printf("Size of char pointer:          %2ld bytes\n",sizeof(bp));
  printf("Size of short int:             %2ld bytes\n",sizeof(c));
  printf("Size of int:                   %2ld bytes\n",sizeof(d));
  printf("Size of long int:              %2ld bytes\n",sizeof(e));
  printf("Size of float:                 %2ld bytes\n",sizeof(f));
  printf("Size of double:                %2ld bytes\n",sizeof(g));
  printf("Size of long double:           %2ld bytes\n",sizeof(h));
  printf("Size of long double pointer:   %2ld bytes\n",sizeof(ip));
  return 0;
}
