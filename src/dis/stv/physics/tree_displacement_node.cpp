/*
 * tree_displacement_node.cpp
 *
 *  Created on: Aug 15, 2010
 *      Author: programmer
 */

#include "tree_displacement_node.h"

TreeDisplacementNode::TreeDisplacementNode(int mass, double locationX):TimeSteppedPhysicsObject(mass),UnidimensionalSpatialObject(locationX), rotation( 0 )
{
}

double TreeDisplacementNode::step(double stepSize)
{
	//printf("...step: ");
	// Apply acceleration
	double change = fabs(this->updateLocation(stepSize));
	// Clear acceleration. Must be recalculated before calling step again.
	this->setAcceleration(0,0);

	return change;
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
	TreeDisplacementNode* copy = new TreeDisplacementNode(this->getMass(),this->getX());
	return copy;
}

void TreeDisplacementNode::applyForce(double forceX, double forceY)
{
	//printf("Applying force <%f>\n",forceX);
	this->applyAcceleration(this->accelerationFromForce(forceX),0);
}

double TreeDisplacementNode::getLocationX()
{
	return this->getX();
}

double TreeDisplacementNode::getLocationY()
{
	return this->getY();
}

void TreeDisplacementNode::setRotation(double rotation)
{
	this->rotation = rotation;
}

double TreeDisplacementNode::getRotation()
{
	return this->rotation;
}
