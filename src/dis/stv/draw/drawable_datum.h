/*
 * drawable_datum.h
 *
 *  Created on: Aug 18, 2010
 *      Author: Mark Christensen
 */

#ifndef DRAWABLE_DATUM_H_
#define DRAWABLE_DATUM_H_

#include "../model/surrogate_tree_node.h"
#include "../system/discursive_system.h"
#include <Magick++.h>

struct MinDrawableDatum
{
	int locationX;
	int locationY;
	double angle;
	int extent;
	int mass;
	double massRatio;
	Magick::Color color;
	SurrogateTreeNode* source;
};


class DrawableDatum
{
private:
	double locationX;
	double locationY;
	double angle;
	double extent;
	double mass;

public:
	DrawableDatum(double locationX, double locationY, double angle, double extent, double mass);
	double getAngle();
	double getLocationX();
	double getLocationY();
	double getExtent();
	double getMass();
};


#endif /* DRAWABLE_DATUM_H_ */
