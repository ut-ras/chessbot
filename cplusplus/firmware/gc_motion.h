//
//  motion.h
//  marlin
//
//  Created by JOSE L CUEVAS on 9/1/19.
//  Copyright © 2019 JOSE L CUEVAS. All rights reserved.
//

#ifndef motion_h
#define motion_h


class gcMotion {
public:

	
	unsigned long lastMotionTime;
	gcMotion();
	
	void setFeedRateWithMultiplier(float feed, float mul);
	void restoreFeedRate();
	
	void setHomePositionForAxis(gcPrintJob &job, int axis);
	
	void hadMotion();
	void pushMove();
	void pushArcMove(char isclockwise);
	void boundEndStops(float target[4]);
	
	void arc(float *position, float *target, float *offset, uint8_t axis_0, uint8_t axis_1, uint8_t axis_linear, float feed_rate, float radius, uint8_t isclockwise, uint8_t extruder);
};
#endif /* motion_h */
