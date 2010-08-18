/*
 * spatial_object.h
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#ifndef SPATIAL_OBJECT_H_
#define SPATIAL_OBJECT_H_

class SpatialObject
{
private:
	double x;
	double y;
	double ddx;
	double ddy;


public:
	SpatialObject();
	SpatialObject(double x, double y);
	void setAcceleration(double x, double y);
	void applyAcceleration(double x, double y);
	void setLocation(double x, double y);
	double getX();
	double getY();
	double getAccelX();
	double getAccelY();
	virtual double updateLocation(double timeDelta) = 0;
};

#endif /* SPATIAL_OBJECT_H_ */
