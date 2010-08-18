/*
 * colonization_leader.h
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#ifndef COLONIZATION_LEADER_H_
#define COLONIZATION_LEADER_H_

class ColonizationLeader
{
private:
	double locationX;
	double locationY;
	double orientation;

public:
	ColonizationLeader(double locationX, double locationY, double orientation);
	double getLocationX();
	double getLocationY();
	double getOrientation();
	void setLocation(double x, double y);
	void setOrientation(double orientation);
};

#endif /* COLONIZATION_LEADER_H_ */
