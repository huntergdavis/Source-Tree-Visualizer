/*
 * tree_displacement_node.h
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#ifndef TREE_DISPLACEMENT_NODE_H_
#define TREE_DISPLACEMENT_NODE_H_

#include "time_stepped_physics_object.h"
#include "tethered_spatial_object.h"

class TreeDisplacementNode : public TimeSteppedPhysicsObject, TetheredSpatialObject
{
public:
	TreeDisplacementNode(int mass, double x, double y, double tetherX, double tetherY, double tetherRadius);
	void step(double stepSize);
	void update(TreeDisplacementNode source);
};

#endif /* TREE_DISPLACEMENT_NODE_H_ */
