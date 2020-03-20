#include "NU32.h"        // cache on, interrupts on, LED/button init, UART init
#include "i2c_display.h"
#include "accel.h"

// the game of snake, on an oled display. eat those pixels!

#define MAX_LEN WIDTH*HEIGHT

typedef struct {
  int head;
  int tail;
  int rows[MAX_LEN];
  int cols[MAX_LEN];
} snake_t; // hold the snake

// direction of the snake
typedef enum {NORTH = 0, EAST = 1, SOUTH = 2, WEST= 3} direction_t; 

// grow the snake in the appropriate direction, returns false if snake has crashed
int snake_grow(snake_t * snake, direction_t dir) { 
  int hrow = snake->rows[snake->head];            
  int hcol = snake->cols[snake->head];

  ++snake->head;
  if(snake->head == MAX_LEN) {
    snake->head = 0;
  }
  switch(dir) {                 // move the snake in the appropriate direction
    case NORTH:
      snake->rows[snake->head] = hrow -1;
      snake->cols[snake->head] = hcol;
      break;
    case SOUTH:
      snake->rows[snake->head] = hrow + 1;
      snake->cols[snake->head] = hcol;
      break;
    case EAST:
      snake->rows[snake->head] = hrow;
      snake->cols[snake->head] = hcol + 1;
      break;
    case WEST:
      snake->rows[snake->head] = hrow;
      snake->cols[snake->head] = hcol -1;
      break;
   }
  // check for collisions with the wall or with itself and return 0, otherwise return 1
  if(snake->rows[snake->head] < 0 || snake->rows[snake->head] >= HEIGHT 
      || snake->cols[snake->head] < 0 || snake->cols[snake->head] >= WIDTH) {
    return 0;
  } else if(display_pixel_get(snake->rows[snake->head],snake->cols[snake->head]) == 1) {
      return 0;
  } else {
    display_pixel_set(snake->rows[snake->head],snake->cols[snake->head],1);
    return 1;
  }

}

void snake_move(snake_t * snake)  { // move the snake by deleting the tail
  display_pixel_set(snake->rows[snake->tail],snake->cols[snake->tail],0);
  ++snake->tail;
  if(snake->tail == MAX_LEN) {
    snake->tail = 0;
  }
}

int main(void) {
  NU32_Startup();
  display_init();
  acc_setup();

  while(1) {
    snake_t snake = {5, 0, {20,20,20,20,20,20},{20,21,22,23,24,25}};
    int dead = 0;
    direction_t dir = EAST;
    char dir_key = 0;
    char buffer[3];
    int i;
    int crow, ccol;
    int eaten = 1;
    int grow = 0;
    short acc[2]; // x and y accleration
    short mag;
    for(i = snake.tail; i <= snake.head; ++i) { // draw the initial snake
      display_pixel_set(snake.rows[i],snake.cols[i],1);
    }
    display_draw();
    acc_read_register(OUT_X_L_M,(unsigned char *)&mag,2);
    srand(mag); // seed the random number generator with the magnetic field 
                // (not the most random, but good enough for this game)
    while(!dead) {
      if(eaten) {
        crow = rand() % HEIGHT;
        ccol = rand() % WIDTH;
        display_pixel_set(crow,ccol,1);
        eaten = 0;
      }
      
      //determine direction based on largest magnitude accel and its direction
      acc_read_register(OUT_X_L_A,(unsigned char *)&acc,4);
      if(abs(acc[0]) > abs(acc[1])) { // move snake in direction of largest acceleration
        if(acc[0] > 0) {              // prevent snake from turning 180 degrees, 
          if(dir != EAST) {           // resulting in an automatic self crash
            dir = WEST;
          }
        } else
          if( dir != WEST) {
            dir = EAST;
          }
      } else {
        if(acc[1] > 0) {
          if( dir != SOUTH) {
            dir = NORTH;
          }
        } else {
          if( dir != NORTH) {
            dir = SOUTH;
          }
        }
      }
      if(snake_grow(&snake,dir)) {
        snake_move(&snake);
      } else if(snake.rows[snake.head] == crow && snake.cols[snake.head] == ccol) {
          eaten = 1;
          grow += 15;
      } else {
        dead = 1;
        display_clear();
      }
      if(grow > 0) {
          snake_grow(&snake,dir);
          --grow;
      }
      display_draw();
    }
  }
  return 0;
}
