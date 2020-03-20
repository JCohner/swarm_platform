#include "NU32.h"
#include "nudsp.h"
// Receives a signal and FIR filter coefficients from the computer.
// filters the signal and ffts the signal and filtered signal, returning the results
// We omit error checking for clarity, but always include it in your own code.

#define SIGNAL_LENGTH 1024
#define FFT_SCALE 10.0
#define FIR_COEFF_SCALE 10.0
#define FIR_SIG_SCALE 10.0
#define NS_PER_TICK 25        // nanoseconds per core clock tick

#define MSG_LEN 128

int main(void) {
  char msg[MSG_LEN];                          // communication buffer
  double fft_orig[SIGNAL_LENGTH] = {};        // fft of the original signal
  double fft_fir[SIGNAL_LENGTH] = {};         // fft of the FIR filtered signal
  double xfir[SIGNAL_LENGTH] = {};            // the FIR filtered signal
  double sig[SIGNAL_LENGTH] = {};             // the signal
  double fir[MAX_ORD] = {};                   // the FIR filter coefficients
  int i = 0;
  int slen, clen;                             // signal and coefficient lengths
  int elapsedticks;                           // duration of FFT in core ticks

  NU32_Startup();

  while (1) {
    // read the signal from the UART. 
    NU32_ReadUART3(msg, MSG_LEN);
    sscanf(msg,"%d",&slen);
    for(i = 0; i < slen; ++i) {
      NU32_ReadUART3(msg, MSG_LEN);
      sscanf(msg,"%f",&sig[i]);
    }
     
    // read the filter coefficients from the UART
    NU32_ReadUART3(msg,MSG_LEN);
    sscanf(msg,"%d", &clen);
    for(i = 0; i < clen; ++i) {
      NU32_ReadUART3(msg,MSG_LEN);
      sscanf(msg,"%f",&fir[i]);
    }

    // FIR filter the signal
    nudsp_fir_1024(xfir, sig, fir, clen, FIR_COEFF_SCALE, FIR_SIG_SCALE);
    
    // FFT the original signal; also time the FFT and send duration in ns
    _CP0_SET_COUNT(0);
    nudsp_fft_1024(fft_orig, sig, FFT_SCALE);
    elapsedticks = _CP0_GET_COUNT();
    sprintf(msg,"%d\r\n",elapsedticks*NS_PER_TICK);  // the time in ns
    NU32_WriteUART3(msg);
    // FFT the FIR signal 
    nudsp_fft_1024(fft_fir, xfir, FFT_SCALE);

    // send the results to the computer
    sprintf(msg,"%d\r\n",SIGNAL_LENGTH);  // send the length
    NU32_WriteUART3(msg);
  for (i = 0; i < SIGNAL_LENGTH; ++i) {
    sprintf(msg,"%12.6f %12.6f %12.6f %12.6f\r\n",sig[i],xfir[i],fft_orig[i],fft_fir[i]);
    NU32_WriteUART3(msg);
   }
  }
  return 0;
}
