/*
 * garbage_sensor.h
 *
 *  Created on: 2018. 11. 21.
 *      Author: user
 */

#ifndef GARBAGE_SENSOR_H_
#define GARBAGE_SENSOR_H_

#include <stdio.h>
#include <tinyara/gpio.h>
#include <time.h>
#include <string.h>
#include <apps/shell/tash.h>
#include <tinyara/config.h>


int getDtValue();
int getMotionValue();

#endif /* GARBAGE_SENSOR_H_ */
