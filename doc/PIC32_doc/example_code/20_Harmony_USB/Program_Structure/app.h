#ifndef APP__H__
#define APP__H__

// The application states.  APP_STATE_INIT is the initial state, used to perform 
// application-specific setup.  Then, during program operation, we enter 
// APP_STATE_WAIT as the timer takes over.
typedef enum { APP_STATE_INIT, APP_STATE_WAIT} APP_STATES; 

// Harmony structure suggests that you place your application-specific data in a struct.
typedef struct {
  APP_STATES state;
  DRV_HANDLE handleTmr;
} APP_DATA;

void APP_Initialize(void);
void APP_Tasks(void);

#endif
