/*
 * spatial_displacement_clustering.h
 *
 *  Created on: Oct 14, 2010
 *      Author: Mark Christensen
 */

#ifndef SPATIAL_DISPLACEMENT_CLUSTERING_H_
#define SPATIAL_DISPLACEMENT_CLUSTERING_H_

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



class SpatialDisplacementClustering : public Decorator
{
public:

	SpatialDisplacementClustering();
	void decorate(SurrogateTreeNode* tree);

private:
	double growthUnit;
	void expand(SurrogateTreeNode* tree, double rootAngle, double rootX, double rootY);
	void cluster(SurrogateTreeNode* tree, double imbalanceThreshold);
	bool shouldCluster(double maxChild, double clusterCount, double clusterAvg, double clusterStddev, double threshold);
	int count(SurrogateTreeNode* tree);
	void insertOrderedBy(vector<SurrogateTreeNode*>* list, SurrogateTreeNode* tree, string property, bool ascending);
};

#endif /* SPATIAL_DISPLACEMENT_CLUSTERING_H_ */
