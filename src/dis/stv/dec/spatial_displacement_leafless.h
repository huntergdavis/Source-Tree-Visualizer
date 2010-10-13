/*
 * spatial_displacement_leafless.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef SPATIAL_DISPLACEMENT_LEAFLESS_H_
#define SPATIAL_DISPLACEMENT_LEAFLESS_H_

#include "./decorator.h"
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



class SpatialDisplacementLeafless : public Decorator
{
public:

	SpatialDisplacementLeafless();
	void decorate(SurrogateTreeNode* tree);

private:
	double growthUnit;
	void expand(SurrogateTreeNode* tree, double rootAngle, double rootX, double rootY);
	int count(SurrogateTreeNode* tree);
	void insertOrderedBy(vector<SurrogateTreeNode*>* list, SurrogateTreeNode* tree, string property, bool ascending);
};

#endif /* SPATIAL_DISPLACEMENT_LEAFLESS_H_ */
