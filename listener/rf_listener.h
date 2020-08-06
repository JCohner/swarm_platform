/*
 * zumo_rf.h
 *
 *  Created on: Mar 2, 2020
 *      Author: jambox
 */

#ifndef RF_LISTENER_H_
#define RF_LISTENER_H_

void rf_setup();
void rf_main();

void rf_post_message(uint32_t data);
#endif /* RF_LISTENER_H_ */
