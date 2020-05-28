#include <math.h>
#include <iostream>
#include "robot.h"

void robot::robot_controller()
{
	if (battery > 0)
	{
		timer++;
		controller();
		switch (motor_command)
		{
			case 1: {
				battery -= .5;
			}
			case 2 | 3:
			{
				battery -= .5;
			}
		}
	}
	else
	{
		speed = 0;
		motor_command = 4;
		color[0] = .3;
		color[1] = .3;
		color[1] = .3;
		tx_request = 0;
	}
}

void robot::robot_init(double x, double y, double t)
{
	static int myid = 0;
	//initalize robot variables
	pos[0] = x;
	pos[1] = y;
	pos[2] = t;
	motor_command = 0;
	timer = rand() / 100;
	incoming_message_flag = 0;
	tx_request = 0;
	// if (id == 0){
	// 	id = rand();
	// }

	id = myid++;
	// printf("assigned id#: %d", id);
	rand();
	motor_error = robot::gauss_rand(timer)*motion_error_std;
	init();
}
