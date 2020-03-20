#include <stdio.h>
#define BYTES_PER_LINE 16

int main(void) {
  FILE *inputp, *outputp;                          // ptrs to in and out files
  int c, count = 0;
  char asc[BYTES_PER_LINE+1], infile[100];

  printf("What binary file do you want the hex rep of? ");
  scanf("%s",infile);                              // get name of input file
  inputp = fopen(infile,"r");                      // open file as "read"
  outputp = fopen("hexdump.txt","w");              // output file is "write"

  asc[BYTES_PER_LINE] = 0;                         // last char is end-string
  while ((c=fgetc(inputp)) != EOF) {               // get byte; end of file?
    fprintf(outputp,"%x%x ",(c >> 4),(c & 0xf));   // print hex rep of byte
    if ((c>=32) && (c<=126)) asc[count] = c;       // put printable chars in asc
    else asc[count] = '.';                         // otherwise put a dot
    count++;
    if (count==BYTES_PER_LINE) {                   // if BYTES_PER_LINE reached
      fprintf(outputp,"  %s\n",asc);               // print ASCII rep, newline
      count = 0;
    }
  }
  if (count!=0) {                                  // print last (short) line
    for (c=0; c<BYTES_PER_LINE-count; c++)         // print extra spaces
      fprintf(outputp,"   ");
    asc[count]=0;                                  // add end-string char to asc
    fprintf(outputp,"  %s\n",asc);                 // print ASCII rep, newline
  }
  fclose(inputp);                                  // close files
  fclose(outputp);
  printf("Printed hexdump.txt.\n");
  return(0);
}
