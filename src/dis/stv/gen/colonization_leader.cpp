/*
 * colonization_leader.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: Mark Christensen
 */

#include "./colonization_leader.h"

ColonizationLeader::ColonizationLeader(double locationX, double locationY, double orientation, SurrogateTreeNode* sourceSet):length(0),startLocationX(locationX), startLocationY(locationY), locationX(locationX), locationY(locationY),orientation(orientation),sourceSet(sourceSet)
{
}

ColonizationLeader::~ColonizationLeader()
{
	if(this->sourceSet != NULL)
	{
		delete(this->sourceSet);
		this->sourceSet = NULL;
	}
}

double ColonizationLeader::getLocationX()
{
	return this->locationX;
}

double ColonizationLeader::getLocationY()
{
	return this->locationY;
}

double ColonizationLeader::getOrientation()
{
	return this->orientation;
}

int ColonizationLeader::getLength()
{
	return this->length;
}

double ColonizationLeader::step(double stepSize)
{
	double newX = this->locationX + (stepSize * cos(this->orientation));
	double newY = this->locationY + (stepSize * sin(this->orientation));

	this->setLocation(newX,newY);
	return this->length;
}

void ColonizationLeader::setLocation(double x, double y)
{
	double dx = x - this->locationX;
	double dy = y - this->locationY;
	double newSegLen = sqrt(dx*dx + dy*dy);
	this->locationX = x;
	this->locationY = y;
	// Update length
	this->length += newSegLen;
}

void ColonizationLeader::setOrientation(double orientation)
{
	this->orientation = orientation;
}

SurrogateTreeNode* ColonizationLeader::getSourceSet()
{
	return this->sourceSet;
}

void ColonizationLeader::setSourceSet(SurrogateTreeNode* newSourceSet)
{
	this->sourceSet = newSourceSet;
}
