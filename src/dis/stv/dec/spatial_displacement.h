/*
 * spatial_displacement.h
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#ifndef SPATIAL_DISPLACEMENT_H_
#define SPATIAL_DISPLACEMENT_H_

#include "decorator.h"
#include "../model/surrogate_tree_node.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <boost/lexical_cast.hpp>
#include "../physics/time_stepped_physics_engine.h"
#include "../physics/tree_displacement_node.h"
#include "../physics/fixed_attractor.h"
#include "../transform/property_inverter.h"
#include "../transform/property_shifter.h"
#include "../system/discursive_system.h"



class SpatialDisplacement : public Decorator
{
public:
	static const double SCALE_WIDTH = 0.9;
	static const double SCALE_HEIGHT = 0.85;

	SpatialDisplacement(int width, int height);
	SpatialDisplacement(int width, int height, double scaleWidth, double scaleHeight);
	void decorate(SurrogateTreeNode* tree);

private:
	int width;
	int height;
	double scaleWidth;
	double scaleHeight;
	void expand(SurrogateTreeNode* tree, double rootAngle, double rootX, double rootY);
	void expand2(SurrogateTreeNode* tree, double rootAngle, double rootX, double rootY, double allowedHeight);
	int count(SurrogateTreeNode* tree);
	void insertOrderedBy(vector<SurrogateTreeNode*>* list, SurrogateTreeNode* tree, string property);
	void transform(SurrogateTreeNode* tree);
};

#endif /* SPATIAL_DISPLACEMENT_H_ */
