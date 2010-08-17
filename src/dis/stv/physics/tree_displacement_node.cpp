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

void TreeDisplacementNode::update(TimeSteppedPhysicsObject* source)
{
	this->setMass(source->getMass());
	if(typeid(source) == typeid(TreeDisplacementNode*))
	{
		TreeDisplacementNode* castSource = static_cast<TreeDisplacementNode*>(source);
		this->setLocation(castSource->getX(), castSource->getY());
		this->setAcceleration(castSource->getAccelX(), castSource->getAccelY());
	}
}

TimeSteppedPhysicsObject* TreeDisplacementNode::clone()
{
	TreeDisplacementNode* copy = new TreeDisplacementNode(this->getMass(),this->getX(),this->getY(),this->getTetherX(), this->getTetherY(), this->getTetherRadius());
	return copy;
}

void TreeDisplacementNode::applyForce(double forceX, double forceY)
{
	this->applyAcceleration(this->accelerationFromForce(forceX), this->accelerationFromForce(forceY));
}
