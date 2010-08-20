/*
 * tree_displacement_node.h
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#ifndef TREE_DISPLACEMENT_NODE_H_
#define TREE_DISPLACEMENT_NODE_H_

#include <math.h>
#include <typeinfo>
#include "time_stepped_physics_object.h"
#include "unidimensional_spatial_object.h"
#include "../system/discursive_system.h"


class TreeDisplacementNode : public TimeSteppedPhysicsObject, public UnidimensionalSpatialObject
{
private:
	double rotation;
public:
	TreeDisplacementNode(int mass, double locationX);
	double step(double stepSize);
	void update(TimeSteppedPhysicsObject* source);
	TimeSteppedPhysicsObject* clone();
	void applyForce(double forceX, double forceY);
	double getLocationX();
	double getLocationY();
	void setRotation(double rotation);
	double getRotation();
};

#endif /* TREE_DISPLACEMENT_NODE_H_ */
