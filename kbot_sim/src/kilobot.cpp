#pragma once
#include "kilolib.h"

class mykilobot : public kilobot
{
	unsigned char distance;
	message_t out_message;
	int rxed=0;
	
	int motion=0;
	long int motion_timer=0;

	int msrx=0;
	// struct mydata {
	// 	//data structure where highest lowest in heard will live, initialize it with current id because it is both the highest and lowest id we have heard
	// 	unsigned short my_x;
	// 	unsigned short my_y;
	// 	unsigned short seed_pos[2][2];
	// 	unsigned char h_count[2];
	// 	unsigned short seed_list[2];
	// 	unsigned char seed_count;
	// };
	// mydata my_info;
	
	struct biosync {
		uint8_t resp_flag = 0;
		uint8_t period_flag = 1;
		uint8_t heard_since_last = 0;

		uint32_t curr_count = 0;
		uint32_t idle_count = 0;

		uint32_t delta_message_time = 10;
		uint32_t message_time;
		uint32_t prev_message_time = 0;

		//for averaged implementation - later
		uint32_t delta_message_time_buff[10];
		uint32_t delta_message_time_average = 10;
		uint32_t num_receives = 0;
		bool receive_buff_full_flag = false;

		//max idle counts before tx
		uint32_t IDLE_MAX = 50;

		void compute_dmt()
		{
			message_time = curr_count;
			delta_message_time = message_time - prev_message_time;
			prev_message_time = message_time;

			if (delta_message_time < 5)
			{
				delta_message_time = 5;
			}

			// printf("dmt: %u\r\n", delta_message_time);
			num_receives = (num_receives + 1) % 10;
			delta_message_time_buff[num_receives] = delta_message_time;
			//deals with averaging the delta message time
	        if (!receive_buff_full_flag)
	        {
	            if (num_receives > 8) {
	                receive_buff_full_flag = true;
	            }
	            delta_message_time_average = 0;
	            
	            for (int i = 0; i < num_receives; i++)
	            {
	                delta_message_time_average += (uint32_t) (1.0/((float) num_receives) *  delta_message_time_buff[i]);
	            }
	        }
	        else //we've filled the buffer
	        {
	            delta_message_time_average = 0;
	            for (int i = 0; i < 10; i++) //TODO!!!! this is a bug in my CCS code, switch from num_receives to CONST 10
	            {
	                delta_message_time_average += (uint32_t) (1.0/((float) 10) *  delta_message_time_buff[i]); //TODO!!!! this is a bug in my CCS code, switch from num_receives to CONST 10
	            }
	        }
		}

		void rx_success()
		{
			//on successful rx set resp flag high
	        idle_count = 0;
	        resp_flag = 1;
	        heard_since_last = 1;
		}

		void tx_success()
		{
			period_flag = 0;
        	resp_flag = 1;
        	heard_since_last = 0;
		}
	};
	biosync sync_info;

	//main loop
	void loop()
	{

	}

	//executed once at start
	unsigned short rid;
	void setup()
	{
		id = id & 0xffff;
		printf("%u\r\n", id);
		set_color(RGB(1,1,1));

		//TODO: figure out how to setup 


	}

	//executed on successfull message send
	void message_tx_success()
	{
		set_color(RGB(1,0,1));
		msrx=1;
	}

	//sends message at fixed rate
	message_t *message_tx()
	{
		// static int count = rand();
		// count--;
		// if (!(count % 50))
		// {
		// 	return &out_message;
		// }
		int return_flag = 0;

		sync_info.curr_count++;
		printf("id: %u ct: %u\r\n", id, sync_info.curr_count);

		// printf("resp_flag: %u, idle_cunt: %u\r\n", sync_info.resp_flag, sync_info.idle_count);

		printf("dmt: %u\r\n", sync_info.delta_message_time);
		printf("dmt ave: %u\r\n", sync_info.delta_message_time_average);
		if(sync_info.resp_flag)
		{
			//in the actual model it calls a sniff here
			sync_info.resp_flag = 0;
		}

		//if no response
		if(!sync_info.resp_flag)
		{
			sync_info.idle_count++;
		}

		//make sure hdmt what we think it is
	    uint32_t hdmt = (uint32_t) sync_info.delta_message_time/2.0;
	    uint32_t it_count = sync_info.idle_count % sync_info.delta_message_time;

		//if we're starting a new period
	    if (it_count == 0)
	    {
	        sync_info.period_flag = 1;
	    }

	    //if the idle_count is greater than halg the delta and you have heard from someone chirp 
	    if ((it_count > hdmt) && (sync_info.period_flag))
	    {
	    	//set flags
	    	sync_info.tx_success();
	    	//send message
	    	return_flag = 1;
	    }

	    if (sync_info.idle_count > sync_info.IDLE_MAX)
	    {
	    	sync_info.idle_count = 0;
	    	sync_info.resp_flag = 1;

	    	return_flag = 1;
	    }

	    // printf("ret flag: %u\n", return_flag);
	    if (return_flag)
	    {
	    	// printf("sending message\r\n");
	    	return &out_message;
	    }
	    else
	    {
			return NULL;
	    }
		// printf("surpp\r\n");
		
	}

	//receives message
	void message_rx(message_t *message, distance_measurement_t *distance_measurement)
	{
		distance = estimate_distance(distance_measurement);
		set_color(RGB(0,1,0));
		// printf("surpp\r\n");
		rxed=1;

		//caches current message time, computes delta message time (dmt)
		sync_info.compute_dmt();
		//handles flags
		sync_info.rx_success();
	}
};
