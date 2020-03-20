#include <math.h>       // C standard library math, for sqrt
#include <stdlib.h>     // for max
#include <string.h>     // for memcpy
#include <dsplib_dsp.h> // for int16, int16c data types and FIR and FFT functions
#include <fftc.h>       // for the FFT twiddle factors, stored in flash
#include "nudsp.h"

#define TWIDDLE fft16c1024 // FFT twiddle factors for int16 (Q15), 1024 signal length
#define LOG2N 10           // log base 2 of the length, assumed to be 1024
#define LEN (1 << LOG2N)   // the length of the buffer
#define QFORMAT (1 << 15)  // multiplication factor to map range (-1,1) to int16 range

// compute the scaling factor to convert an array of doubles to int16 (Q15)
// The scaling is performed so that the largest magnitude number in din
// is mapped to 1/div; thus the divisor gives extra headroom to avoid overflow

double nudsp_qform_scale(double * din, int len, double div) {
  int i;
  double maxm = 0.0;
  
  for (i = 0; i< len; ++i) {
    maxm = max(maxm, fabs(din[i]));
  }
  return (double)QFORMAT/(maxm * div);  
}

// Performs an FFT on din (assuming it is 1024 long), returning its magnitude in dout 
// dout - pointer to array where answer will be stored
// din - pointer to double array to be analyzed
// div - input scaling factor.  max magnitude input is mapped to 1/div

void nudsp_fft_1024(double *dout, double *din, double div)
{
  int i = 0;
  int16c twiddle[LEN/2];
  int16c dest[LEN], src[LEN];
  int16c scratch[LEN];
  double scale = nudsp_qform_scale(din,LEN,div);
    
  for (i=0; i< LEN; i++) {                        // convert to int16 (Q15)
    src[i].re = (int) (din[i] * scale);
    src[i].im = 0;
  }
  memcpy(twiddle, TWIDDLE, sizeof(twiddle));      // copy the twiddle factors to RAM
  mips_fft16(dest, src, twiddle, scratch, LOG2N); // perform FFT
  for (i = 0; i < LEN; i++) {                     // convert the results back to doubles
    double re = dest[i].re / scale;
    double im = dest[i].im / scale;
    dout[i] = sqrt(re*re + im*im);
  }
}

// Perform a finite impulse response filter of a signal that is 1024 samples long
// dout - pointer to result array
// din - pointer to input array
// c - pointer to coefficient array
// nc - the number of coefficients 
// div_c - for scaling the coefficients 
// The maximum magnitude coefficient is mapped to 1/div_c in int16 (Q15)
// div_sig - for scaling the input signal
// The maximum magnitude input is mapped to 1/div_sig in int16 (Q15)
void 
  nudsp_fir_1024(double *dout,double *din,double *c,int nc,double div_c,double div_sig) 
{
  int16 fir_coeffs[MAX_ORD], fir_coeffs2x[2*MAX_ORD];
  int16 delay[MAX_ORD] = {};
  int16 din16[LEN], dout16[LEN];
  int i=0;
  double scale_c = nudsp_qform_scale(c, nc, div_c);      // scale coeffs to Q15
  double scale_s = nudsp_qform_scale(din, LEN, div_sig); // scale signal to Q15
  double scale = 0.0;
                                                    
  for (i = 0; i< nc; ++i) {                              // convert FIR coeffs to Q15
    fir_coeffs[i] = (int) (c[i]*scale_c);
  }
  for (i = 0; i<LEN; i++) {                              // convert input signal to Q15
    din16[i] = (int) (din[i]*scale_s);
  }
  mips_fir16_setup(fir_coeffs2x, fir_coeffs, nc);        // set up the filter
  mips_fir16(dout16, din16, fir_coeffs2x, delay, LEN, nc, 0);  // run the filter
  scale = (double)QFORMAT/(scale_c*scale_s);             // convert back to doubles
  for (i = 0; i<LEN; i++) {
    dout[i] = dout16[i]*scale;    
  }
}
