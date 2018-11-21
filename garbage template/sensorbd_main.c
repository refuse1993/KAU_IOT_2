/****************************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/


#include "garbage_sensor.h"
#include "wifi.h"

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int sensorbd_main(int argc, FAR char *argv[])
#endif
{
	int dt11Data[2];
	int dt11Read;
	float distance;

	while(1){
		dt11Read = getDtValue(dt11Data);
		printf("Temperature: %d, Humidity: %d\n", dt11Data[0], dt11Data[1]);
		up_mdelay(5000);
		getMotionValue(&distance);
		printf("Distance: %.2f cm\n", distance);
		up_mdelay(5000);
	}
}
