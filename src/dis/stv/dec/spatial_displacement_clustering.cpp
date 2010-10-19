/*
 * spatial_displacement_clustering.cpp
 *
 *  Created on: Oct 14, 2010
 *      Author: Mark Christensen
 */

#include "./spatial_displacement_clustering.h"



using namespace std;

SpatialDisplacementClustering::SpatialDisplacementClustering():growthUnit(20.0)
{
}

void SpatialDisplacementClustering::decorate(SurrogateTreeNode* tree)
{
	// Iterate over each node with children
	// First, "count".  Calculates child center of mass and subtree depth
	this->count(tree);
	// Second, cluster node in heavily imbalanced directories
	this->cluster(tree,0.5);
	// Third, float weighted surrogate nodes into position
	// Max tree depth
	this->expand(tree,(3.14159/2),0,0);
}

// threshold is percentage of maxChild that is target for cluster size.
// threshold = 1.0 means cluster() will try to make clusters of size maxChild
bool SpatialDisplacementClustering::shouldCluster(double maxChild, double clusterCount, double clusterAvg, double clusterStddev, double threshold)
{
	return (clusterCount > 1) && ((maxChild * clusterCount * threshold) > (clusterAvg + 3 * clusterStddev));
}

void SpatialDisplacementClustering::cluster(SurrogateTreeNode* tree, double imbalanceThreshold)
{
	int depth = atoi(tree->data[TreeNodeKey::DEPTH].c_str());
	if(depth > 2 && tree->children->size() > 0)
	{
		SurrogateTreeNode* node;
		int maxChild = -DBL_MAX;
		int childSize;
		int mass = 0;
		int numToCluster = 0;
		double stddev = 0;
		double avg;
		// First loop: calc metrics (max,stddev)
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children->begin(); iter != tree->children->end(); ++iter)
		{
			node = *iter;
			childSize = atoi(node->data[TreeNodeKey::SIZE].c_str());

			if(childSize >= maxChild)
			{
				maxChild = childSize;
			}
			else
			{
				// Only cluster directories
				if(node->children->size() > 0)
				{
					numToCluster++;
					mass += childSize;
				}
			}
		}
		if(numToCluster > 0)
		{
			avg = mass / (double)numToCluster;
			double weightedSum = 0;
			// Second loop: calc metrics (max,stddev)
			for(vector<SurrogateTreeNode*>::iterator iter = tree->children->begin(); iter != tree->children->end(); ++iter)
			{
				node = *iter;
				childSize = atoi(node->data[TreeNodeKey::SIZE].c_str());

				if(childSize < maxChild && node->children->size() > 0)
				{
					weightedSum += pow(childSize - avg, 2);
				}
			}
			stddev = sqrt(weightedSum/numToCluster);
		}
		// If we need to cluster
		if(this->shouldCluster(maxChild, numToCluster, avg, stddev, imbalanceThreshold))
		{
			printf("Decision made to cluster '%s' with <%d, %d, %f, %f, %f>\n", tree->data[TreeNodeKey::NAME].c_str(), maxChild, numToCluster, avg, stddev, imbalanceThreshold);
			// Holds new child set for clustered old children
			vector<SurrogateTreeNode*> clustered;
			SurrogateTreeNode* currentNode = NULL;
			int currentNodeSize = 0;

			// Third loop: extract old children into new clusters
			double clusterTarget = maxChild * imbalanceThreshold;
			for(vector<SurrogateTreeNode*>::iterator iter = tree->children->begin(); iter != tree->children->end(); ++iter)
			{
				node = *iter;
				childSize = atoi(node->data[TreeNodeKey::SIZE].c_str());

				// Max nodes are moved directly to new child set
				if(childSize == maxChild || node->children->size() == 0)
				{
					clustered.push_back(node);
					printf("-- Pushed non-clusterable item\n");
				}
				else
				{
					if(currentNode == NULL)
					{
						currentNode = new SurrogateTreeNode();
						currentNode->set(TreeNodeKey::DEPTH,atoi(node->data[TreeNodeKey::DEPTH].c_str()));
						currentNode->set(TreeNodeKey::CREATION_TIME,atol(node->data[TreeNodeKey::CREATION_TIME].c_str()));
						currentNode->set(TreeNodeKey::REVISIONCREATED,atol(node->data[TreeNodeKey::REVISIONCREATED].c_str()));
						currentNode->set(TreeNodeKey::COLOR,string(tree->data[TreeNodeKey::COLOR]));
					}
					currentNode->children->push_back(node);
					currentNodeSize += childSize;
					if(currentNodeSize >= clusterTarget && currentNode->children->size() > 1)
					{
						printf("-- New cluster of size %d/%d and color '%s'\n", currentNode->children->size(), currentNodeSize,currentNode->data[TreeNodeKey::COLOR].c_str());

						currentNode->set(TreeNodeKey::SIZE,currentNodeSize);
						clustered.push_back(currentNode);
						currentNode = NULL;
						currentNodeSize = 0;
					}
				}
			}
			// If there is an incomplete node, add it to new child set
			if(currentNode != NULL)
			{
				printf("-- New cluster of size %d/%d and color '%s'\n", currentNode->children->size(), currentNodeSize,currentNode->data[TreeNodeKey::COLOR].c_str());
				currentNode->set(TreeNodeKey::SIZE,currentNodeSize);
				clustered.push_back(currentNode);
				currentNode = NULL;
			}

			// Clear old child list
			tree->children->clear();

			// Copy new children to current node
			tree->children->insert(tree->children->begin(), clustered.begin(), clustered.end());

			// Clear new list
			clustered.clear();
		}

		// Fourth loop: recursive over all children
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children->begin(); iter != tree->children->end(); ++iter)
		{
			node = *iter;
			this->cluster(node, imbalanceThreshold);
		}
	}
}

// Sorted in increasing order
void SpatialDisplacementClustering::insertOrderedBy(vector<SurrogateTreeNode*>* list, SurrogateTreeNode* tree, string property, bool ascending = true)
{
	SurrogateTreeNode* node;
	bool inserted = false;
	long comp = atol(tree->data[property].c_str());
	long curr = 0;
	for(vector<SurrogateTreeNode*>::iterator iter = list->begin(); iter != list->end(); ++iter)
	{
		node = *iter;
		curr = atol(node->data[property].c_str());
		if((ascending && comp < curr) || (!ascending && comp > curr))
		{
			list->insert(iter,tree);
			inserted = true;
			break;
		}
	}
	if(!inserted)
	{
		list->push_back(tree);
	}
}

void SpatialDisplacementClustering::expand(SurrogateTreeNode* tree, double rootAngle, double rootX, double rootY)
{
	// Set this nodes location
	tree->set(TreeNodeKey::X, boost::lexical_cast<string>(rootX));
	tree->set(TreeNodeKey::Y, boost::lexical_cast<string>(rootY));
	tree->set(TreeNodeKey::ANGLE, boost::lexical_cast<string>(rootAngle));
	DiscursiveDebugPrint("sdc","  (%s) @ <%f,%f,%f>\n",tree->data[TreeNodeKey::NAME].c_str(),rootX,rootY,rootAngle);

	if(tree->children->size() > 0)
	{
		// Determine initial layout based on creation time of child nodes.
		// Files are split into different list than directories.
		vector<SurrogateTreeNode*> files;
		vector<SurrogateTreeNode*> dirs;
		SurrogateTreeNode* node = NULL;
		int maxChild = 1;
		int childSize;
		int mass = 0;
		int children = 0;
		int subtrees = 0;
		int leaves = 0;
		double minChildMass = DBL_MAX;
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children->begin(); iter != tree->children->end(); ++iter)
		{
			node = *iter;
			// Directory
			if(node->children->size() > 0)
			{
				subtrees++;
				this->insertOrderedBy(&dirs,node,TreeNodeKey::REVISIONCREATED,false);
				childSize = atoi(node->data[TreeNodeKey::SIZE].c_str());
				mass += childSize;
				if(childSize > maxChild )
				{
					maxChild = childSize;
				}
				if(childSize < minChildMass)
				{
					minChildMass = childSize;
				}
			}
			// File
			else
			{
				leaves++;
			}
		}
		children = subtrees;

		// Map for retrieving resutls
		unordered_map<SurrogateTreeNode*,int> pairs;
		double masses[children];
		double positions[children];
		memset(positions,0,children*sizeof(double));
		// Layout directories first
		// Start left if even number of items.
		bool left = children % 2 != 0;
		int center = children / 2;
		int dist = 0;
		int i = 0;
		int location;
		int nodeMass;
		for(; i < subtrees; i++)
		{
			dist = (i+1)/2;
			nodeMass = atoi(dirs[i]->data[TreeNodeKey::SIZE].c_str());
			if(left)
			{
				location = center - dist;
			}
			else
			{
				location = center + dist;
			}
			masses[location] = nodeMass;
			pairs[dirs[i]] = location;
			for(int k = location; k < children; k++)
			{
				positions[k] += nodeMass;
			}
			left = !left;
		}

		// Calculate spacing to range [0,splay]
		double deltaSplay = 0;
		double splayModifier = 16.0;
		splayModifier /= (2.0 * pow(2,log(children+2)));
		if(splayModifier < 1)
		{
			splayModifier = 1;
		}
		double splay = 3.14159 / splayModifier;
		double com;
		double max = 0;
		if(mass > minChildMass)
		{
			deltaSplay = splay / (mass - minChildMass);
		}

		DiscursiveDebugPrint("sdc","Dirs/Leaves [%d/%d], Splay [%f], Splay modifier [%f], Delta splay [%f]\n",subtrees,leaves,splay, splayModifier, deltaSplay);

		DiscursiveDebugPrint("sdc","Subtree mass effects (%s @ %d) [",tree->data[TreeNodeKey::NAME].c_str(),mass);
		for(i = 0; i < children; i++)
		{
			DiscursiveDebugPrint("sdc","%f,",positions[i]);
		}
		DiscursiveDebugPrint("sdc","] @ %f\n", deltaSplay);

		// Convert masses to initial locations
		// Record bounds
		positions[0] = 0;
		com = 0;
		for(i = 1; i < children; i++)
		{
			positions[i] = (positions[i] - minChildMass)*deltaSplay;
			com += (positions[i] * masses[i]);

			if(positions[i] > max)
			{
				max = positions[i];
			}
		}
		// Final part of CoM calculation
		if(mass > 0)
		{
			com /= mass;
		}

		// Balance tree
		double divergence = com - (3.14159/2);
//		printf("CoM: %f, Max: %f, Divergence: %f\n", com, max, divergence);


		// Calculate first 2 segments of branch
		// Branch base first
		double length = 7.5 * this->growthUnit;
//		if(subtrees == 0)
//		{
//			// If only leaves, no branch spacing.
//			length = 0;
//		}

		// Add leaf branch spacing
		// This uses full child node count and not 'leaves'
		// because of a calc issue in TrapezoidBlocks
//		length += (ceil(leaves / 5.0) + 1) * (2 * this->growthUnit);
		length += (ceil(tree->children->size() / 5.0) + 1) * (2 * this->growthUnit);

		// Transform positions to arc
		//double arcRadius = 2.0 * maxChild * this->growthUnit;
		double arcRadius = 3 * length * log(subtrees+2);
		double ratio;
		double angle;

		// Calculate fake root location
		double fakeRootX = rootX + (length * cos(rootAngle));
		double fakeRootY = rootY + (length * sin(rootAngle));

		// Controls width of fan-out.  < 1 : Wide tree
		//							   > 1 : Narrow tree
		double widthHeightScaleFactor = 1;
		// Transform positions to arc and Update new positions
		// Dirs first
		double xSum = 0;
		double ySum = 0;
		int lmass;
		double massSum = 0;
		bool spread = (subtrees > 3);
		double stepPer = 7.5;
		int centerIndex = (subtrees / 2);
		double spreadStep = max * stepPer;
		// Bias root upwards
		rootAngle = (3.14159/2);
		for(i = 0; i < subtrees; i++)
		{
			node = dirs[i];
			ratio = atoi(node->data[TreeNodeKey::SIZE].c_str())/(double)maxChild;
			angle = rootAngle - (positions[pairs[node]] - com);
			if(rootAngle + com > 3.14159)
			{
				angle *= (3.14159) / (rootAngle + com);
			}
			double newX = fakeRootX + (ratio * arcRadius * cos(angle));
			if(spread)
			{
				newX += (i - centerIndex) * spreadStep;
			}
			double newY = fakeRootY + (ratio * arcRadius * widthHeightScaleFactor * sin(angle));
			lmass = 1.0 * atoi(node->data[TreeNodeKey::SIZE].c_str()) / (double)maxChild;
			xSum += (newX * lmass);
			ySum += (newY * lmass);
			massSum += lmass;
			// Run expand on child
			double childRot = angle;//(3.14159/2);//angle;// + ((3.14159/2)-angle)/2;
			this->expand(node,childRot,newX,newY);
		}
		if(massSum != 0)
		{
			xSum /= massSum;
			ySum /= massSum;
			tree->set(TreeNodeKey::SCOMX,boost::lexical_cast<string>(xSum));
			tree->set(TreeNodeKey::SCOMY,boost::lexical_cast<string>(ySum));
		}
		else
		{
			xSum = fakeRootX;
			ySum = fakeRootY;
			tree->set(TreeNodeKey::SCOMX,boost::lexical_cast<string>(rootX));
			tree->set(TreeNodeKey::SCOMY,boost::lexical_cast<string>(rootY));
		}
		// Log branch length
		// Add sub-branch spacing
		double dx = xSum - fakeRootX;
		double dy = ySum - fakeRootY;
		double branchLen = sqrt(dx*dx + dy*dy);
		length += branchLen;
		node->set(TreeNodeKey::LENGTH,boost::lexical_cast<string>(length));
	}
}

int SpatialDisplacementClustering::count(SurrogateTreeNode* tree)
{
	// Count all children plus ourselves (initial 1)
	int sum = 1;
	int maxDepth = 0;
	int childDepth = 0;
	for(vector<SurrogateTreeNode*>::iterator iter = tree->children->begin(); iter != tree->children->end(); ++iter)
	{
		sum += this->count(*iter);
		childDepth = atoi((*iter)->data[TreeNodeKey::DEPTH].c_str());
		if(childDepth > maxDepth)
		{
			maxDepth = childDepth;
		}
	}
	// Assign count to data
	tree->set(TreeNodeKey::SIZE, boost::lexical_cast<string>(sum));
	DiscursiveDebugPrint("sdc count","Size of tree %s is %s in this and %d sublevels\n",tree->data[TreeNodeKey::NAME].c_str(),tree->data[TreeNodeKey::SIZE].c_str(),maxDepth);
	tree->set(TreeNodeKey::DEPTH, boost::lexical_cast<string>(maxDepth + 1));
	return sum;
}
