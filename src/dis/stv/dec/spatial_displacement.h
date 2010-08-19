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
#include <stdio.h>
#include <float.h>
#include <boost/lexical_cast.hpp>
#include "../physics/time_stepped_physics_engine.h"
#include "../physics/tree_displacement_node.h"
#include "../physics/fixed_attractor.h"


class SpatialDisplacement : public Decorator
{
public:
	SpatialDisplacement(double rootX, double rootY);
	void decorate(SurrogateTreeNode* tree);

private:
	double rootX;
	double rootY;
	void expand(SurrogateTreeNode* tree, double rootAngle, double rootX, double rootY);
	int count(SurrogateTreeNode* tree);
	void insertOrderedBy(vector<SurrogateTreeNode*>* list, SurrogateTreeNode* tree, string property);
};

#endif /* SPATIAL_DISPLACEMENT_H_ */
