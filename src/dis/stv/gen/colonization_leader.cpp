/*
 * colonization_leader.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#include "colonization_leader.h"

ColonizationLeader::ColonizationLeader(double locationX, double locationY, double orientation, SurrogateTreeNode* sourceSet):locationX(locationX), locationY(locationY),orientation(orientation),sourceSet(sourceSet)
{
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

void ColonizationLeader::setLocation(double x, double y)
{
	this->locationX = x;
	this->locationY = y;
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