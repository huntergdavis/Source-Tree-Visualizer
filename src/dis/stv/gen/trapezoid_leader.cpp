/*
 * trapezoid_leader.cpp
 *
 *  Created on: Oct 05, 2010
 *      Author: Mark Christensen
 */

#include "./trapezoid_leader.h"

TrapezoidLeader::TrapezoidLeader(double baseX, double baseY, double baseWidth, double orientation, double initSpacer, SurrogateTreeNode* sourceSet):length(initSpacer), baseX(baseX), baseY(baseY), baseWidth(baseWidth), endWidth(0.5*baseWidth), orientation(orientation),initSpacer(initSpacer),sourceSet(sourceSet),leaves(new vector<SurrogateTreeNode*>())
{
}

TrapezoidLeader::~TrapezoidLeader()
{
	if(this->sourceSet != NULL)
	{
		delete(this->sourceSet);
		this->sourceSet = NULL;
	}
	if(this->leaves != NULL)
	{
		this->leaves->clear();
		delete(this->leaves);
		this->leaves = NULL;
	}
}

double TrapezoidLeader::getBaseX()
{
	return this->baseX;
}

double TrapezoidLeader::getBaseY()
{
	return this->baseY;
}

double TrapezoidLeader::getBaseWidth()
{
	return this->baseWidth;
}

double TrapezoidLeader::getEndWidth()
{
	return this->endWidth;
}

void TrapezoidLeader::setEndWidth(double endWidth)
{
	this->endWidth = endWidth;
}

double TrapezoidLeader::getOrientation()
{
	return this->orientation;
}

int TrapezoidLeader::getLength()
{
	return this->length;
}

double TrapezoidLeader::step(double stepSize)
{
	this->length += stepSize;
	return this->length;
}

double TrapezoidLeader::getX()
{
	return this->baseX + (this->length * cos(this->orientation));
}

double TrapezoidLeader::getY()
{
	return this->baseY - (this->length * sin(this->orientation));
}

void TrapezoidLeader::setOrientation(double orientation)
{
	this->orientation = orientation;
}

SurrogateTreeNode* TrapezoidLeader::getSourceSet()
{
	return this->sourceSet;
}

void TrapezoidLeader::setSourceSet(SurrogateTreeNode* newSourceSet)
{
	this->sourceSet = newSourceSet;
}
