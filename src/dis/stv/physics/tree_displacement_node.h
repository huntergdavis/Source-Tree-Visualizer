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

class TreeDisplacementNode : public TimeSteppedPhysicsObject, public UnidimensionalSpatialObject
{
public:
	TreeDisplacementNode(int mass, double locationX);
	double step(double stepSize);
	void update(TimeSteppedPhysicsObject* source);
	TimeSteppedPhysicsObject* clone();
	void applyForce(double forceX, double forceY);
	double getLocationX();
	double getLocationY();
};

#endif /* TREE_DISPLACEMENT_NODE_H_ */
