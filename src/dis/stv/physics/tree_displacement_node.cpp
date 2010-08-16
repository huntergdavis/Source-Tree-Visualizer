/*
 * tree_displacement_node.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "tree_displacement_node.h"

TreeDisplacementNode::TreeDisplacementNode(int mass, double x, double y, double tetherX, double tetherY, double tetherRadius):TimeSteppedPhysicsObject(mass),TetheredSpatialObject(x,y,tetherX, tetherY, tetherRadius)
{
}

void TreeDisplacementNode::step(double stepSize)
{
	// Apply acceleration
	this->updateLocation(stepSize);
	// Clear acceleration. Must be recalculated before calling step again.
	this->setAcceleration(0,0);
}

void TreeDisplacementNode::update(TreeDisplacementNode source)
{
	this->setMass(source.getMass());
	this->setLocation(source.getX(), source.getY());
	this->setAcceleration(source.getAccelX(), source.getAccelY());
}
