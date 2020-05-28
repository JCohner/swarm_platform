#pragma once
#include <iostream>
#ifndef ROBOT_H
#define ROBOT_H
#define motion_error_std .007
#define PI 3.14159265358979324
#define GAUSS 10000
#define right 2
#define left 3
#define sensor_lightsource 1
#define radius 32
#define X 0
#define Y 1
#define T 2
#define SECOND 32

class robot
{
public:
	int id = 0;
	double pos[3];//x,y,theta position in real world, dont use these in controller, thats cheating!!
	double motor_error;//value of how motors differ from ideal, dont use these, thats cheating!!
	double comm_range = 60; //communication range between robots
	double color[3]; //robot color output, values 0-1
	
					 //robot commanded motion 1=forward, 2=cw rotation, 3=ccw rotation, 4=stop
	int motor_command;

	double dest[3] = { -1,-1,-1 };

	void robot_init(double, double, double);
	//overided by init init defined in kilolib.h
	virtual void init() = 0;

	//robots internal timer
	int timer;

	//must implement the controller
	void robot_controller();
	virtual void controller() = 0;

	virtual void sensing(int, int[], int[], int[], int[]) = 0;

	//flag set to 1 when robot wants to transmitt
	int tx_request;

	//flag set to 1 when new message received
	int incoming_message_flag;

	virtual void *get_message() = 0;

	double speed=.5;

	double battery = -1;

	virtual char *get_debug_info(char *buffer, char *rt) = 0;

	virtual double comm_out_criteria(double destination_x, double destination_y, int sd) = 0;
	virtual bool comm_in_criteria(double source_x, double source_y, double distance, void *cd) = 0;

	//useful  
	static double distance(double x1, double y1, double x2, double y2)
	{
		double x = x1 - x2;
		double y = y1 - y2;
		double s = pow(x, 2) + pow(y, 2);
		return sqrt(s);
	}
	static double find_theta(double x1, double y1, double x2, double y2)
	{
		if (x1 == x2) return 0;
		double x = x2 - x1;
		double y = y2 - y1;

		if (x >= 0 && y >= 0)
		{
			return atan(y / x);
		}
		if (x < 0 && y < 0)
		{
			return atan(y / x) + PI;
		}
		if (x < 0 && y > 0)
		{
			return atan(abs(x) / y) + PI / 2;
		}
		return atan(x / abs(y)) + PI / 2 * 3;
	}
	static double gauss_rand(int timer)
	{
		static double pseudogaus_rand[GAUSS + 1];
		if (pseudogaus_rand[GAUSS] == 1)
		{
			return pseudogaus_rand[timer % GAUSS];
		}
		for (int i = 0; i < GAUSS;i++)
		{
			pseudogaus_rand[i] = gaussrand();
		};
		pseudogaus_rand[GAUSS] = 1;
		return pseudogaus_rand[timer % GAUSS];
	}
	static double tetha_diff(double t1, double t2)
	{
		double diff = t1 - t2;
		if (diff < -PI)
		{
			diff += 2 * PI;
		}
		else if (diff>PI)
		{
			diff -= 2 * PI;
		}
		return diff;
	}
	virtual void received()=0;


private:
	static double gaussrand()
	{
		static double V1, V2, S;
		static int phase = 0;
		double x;

		if (phase == 0) {
			do {
				double U1 = (double)rand() / RAND_MAX;
				double U2 = (double)rand() / RAND_MAX;

				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} while (S >= 1 || S == 0);

			x = V1 * sqrt(-2 * log(S) / S);
		}
		else
			x = V2 * sqrt(-2 * log(S) / S);

		phase = 1 - phase;

		return x;
	}
};
#endif
