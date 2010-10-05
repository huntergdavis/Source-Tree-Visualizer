/*
 * spatial_displacement_leafless.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#include "spatial_displacement_leafless.h"
#include "../system/discursive_system.h"


using namespace std;

SpatialDisplacementLeafless::SpatialDisplacementLeafless():growthUnit(10.0)
{
}

void SpatialDisplacementLeafless::decorate(SurrogateTreeNode* tree)
{
	// Iterate over each node with children
	// First, "count".  Calculates child center of mass and subtree depth
	this->count(tree);
	// Second, float weighted surrogate nodes into position
	// Max tree depth
	double maxDepth = tree->findMax(TreeNodeKey::DEPTH);
	this->expand(tree,(3.14159/2),0,0);
}

// Sorted in increasing order
void SpatialDisplacementLeafless::insertOrderedBy(vector<SurrogateTreeNode*>* list, SurrogateTreeNode* tree, string property)
{
	SurrogateTreeNode* node;
	bool inserted = false;
	long comp = atol(tree->data[property].c_str());
	long curr = 0;
	for(vector<SurrogateTreeNode*>::iterator iter = list->begin(); iter != list->end(); ++iter)
	{
		node = *iter;
		curr = atol(node->data[property].c_str());
		if(comp < curr)
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

void SpatialDisplacementLeafless::expand(SurrogateTreeNode* tree, double rootAngle, double rootX, double rootY)
{
	bool includeFiles = true;
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
				this->insertOrderedBy(&dirs,node,TreeNodeKey::SIZE);
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
			else if(includeFiles)
			{
				leaves++;
				this->insertOrderedBy(&files,node,TreeNodeKey::CREATION_TIME);
				mass++;
				if(minChildMass > 1)
				{
					minChildMass = 1;
				}
			}
		}
		children = subtrees + leaves;

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
		for(; i < (int)dirs.size(); i++)
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
		// Now layout files
//		for(int j = 0; j < (int)files.size(); j++)
//		{
//			dist = ((i + j + 1)/2);
//			nodeMass = 1;
//			if(left)
//			{
//				location = center - dist;
//			}
//			else
//			{
//				location = center + dist;
//			}
//			masses[location] = nodeMass;
//			pairs[files[j]] = location;
//			for(int k = location; k < children; k++)
//			{
//				positions[k] += nodeMass;
//			}
//			left = !left;
//		}

		// Calculate spacing to range [0,splay]
		double deltaSplay = 0;
		double splay = 3.14159 / 2;
		double com;
		double max = 0;
		if(mass > minChildMass)
		{
			deltaSplay = splay / (mass - minChildMass);
		}

		DiscursiveDebugPrint("Subtree mass effects [");
		for(i = 0; i < children; i++)
		{
			DiscursiveDebugPrint("%f,",positions[i]);
		}
		DiscursiveDebugPrint("] @ %f\n", deltaSplay);

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
		com /= mass;

		// Balance tree
//		double balancedCom = com;
//		double divergence = balancedCom - (max/2);
//		double scale;
//		bool shortenLeft;
//		printf("CoM: %f, Max: %f, Divergence: %f\n", balancedCom, max, divergence);
//		int step = 0;
//		while(fabs(divergence) > 3.14159 / (50-step))
//		{
//			// Shorten left half
//			if(divergence > 0)
//			{
//				shortenLeft = true;
//				scale = (max - balancedCom)/balancedCom;
//			}
//			// Shorten right half
//			else
//			{
//				shortenLeft = false;
//				scale = balancedCom/(max - balancedCom);
//			}
//			com = 0;
//			for(i = 1; i < children; i++)
//			{
//				// Adjust position
//				if(shortenLeft && positions[i] < balancedCom)
//				{
//					positions[i] = (positions[i] - divergence) * scale;
//				}
//				else if(!shortenLeft && positions[i] > balancedCom)
//				{
//					positions[i] = ((positions[i] - balancedCom) * scale) + balancedCom;
//				}
//				else if(shortenLeft && positions[i] > balancedCom)
//				{
//					positions[i] -= (2 * balancedCom - max);
//				}
////				else
////				{
////					positions[i] = ;
////				}
//
//				// Aggregate new position to calc new CoM
//				com += (positions[i] * masses[i]);
//			}
//			max = 2*(max - balancedCom);
//			balancedCom = com/mass;
//			divergence = balancedCom - (max/2);
//			step++;
////			printf("CoM: %f, Max: %f, Divergence: %f\n", balancedCom, max, divergence);
//		}

		// Calculate first 2 segments of branch
		// Branch base first
		double length = 2*this->growthUnit;
		// Add leaf branch spacing
		length += ( 2 * (leaves / 5.0) ) * this->growthUnit;


		// Transform positions to arc
		//double arcRadius = 10.0;
//		double deltaAngle = splay / children;
//		int maxDepth = (int)tree->findMax(TreeNodeKey::DEPTH);
//		int depth;
//		double arcRadius = allowedHeight/maxDepth;
		double arcRadius = maxChild * this->growthUnit;
		double ratio;
		double angle;

		// Calculate fake root location
		double fakeRootX = rootX + (length * cos(rootAngle));
		double fakeRootY = rootY + (length * sin(rootAngle));

		// Controls width of fan-out.  < 1 : Wide tree
		//							   > 1 : Narrow tree
		double widthHeightScaleFactor = 1.2;
		// Transform positions to arc and Update new positions
		// Dirs first
		double xSum = 0;
		double ySum = 0;
		int lmass;
		for(i = 0; i < (int)dirs.size(); i++)
		{
			node = dirs[i];
//			depth = atoi(node->data[TreeNodeKey::DEPTH].c_str());
//			ratio = depth / (double)maxDepth;
			ratio = atoi(node->data[TreeNodeKey::SIZE].c_str())/(double)maxChild;
			angle = rootAngle - (positions[pairs[node]] - com);
			double newX = fakeRootX + (ratio * arcRadius * cos(angle));
			double newY = fakeRootY + (ratio * arcRadius * widthHeightScaleFactor * sin(angle));
			lmass = atoi(node->data[TreeNodeKey::SIZE].c_str());
			xSum += (newX * lmass);
			ySum += (newY * lmass);
//			double newX = ratio * arcRadius * cos(angle);
//			double newY = ratio * arcRadius * widthHeightScaleFactor * sin(angle);
			node->set(TreeNodeKey::X, boost::lexical_cast<string>(newX));
			node->set(TreeNodeKey::Y, boost::lexical_cast<string>(newY));
			// Run expand on child
			double childRot = (3.14159/2);//angle;// + ((3.14159/2)-angle)/2;
			this->expand(node,childRot,newX,newY);
		}
		// Then files
		if(includeFiles)
		{
			for(int j = 0; j < (int)files.size(); j++)
			{
				node = files[j];
//				depth = atoi(node->data[TreeNodeKey::DEPTH].c_str());
//				ratio = depth / (double)maxDepth;
				ratio = 1 / (double)maxChild;
				angle = rootAngle - (positions[pairs[node]] - com);
				double newX = fakeRootX + (ratio * arcRadius * cos(angle));
				double newY = fakeRootY + (ratio * arcRadius * widthHeightScaleFactor * sin(angle));
				lmass = 1;
				xSum += (newX * lmass);
				ySum += (newY * lmass);
				node->set(TreeNodeKey::X, boost::lexical_cast<string>(newX));
				node->set(TreeNodeKey::Y, boost::lexical_cast<string>(newY));
			}
		}
		xSum /= mass;
		ySum /= mass;
		node->set(TreeNodeKey::SCOMX,boost::lexical_cast<string>(xSum));
		node->set(TreeNodeKey::SCOMY,boost::lexical_cast<string>(ySum));
		// Log branch length
		// Add sub-branch spacing
		double dx = fakeRootX - xSum;
		double dy = fakeRootY - ySum;
		double branchLen = sqrt(dx*dx + dy*dy);
		length += branchLen;
		node->set(TreeNodeKey::LENGTH,boost::lexical_cast<string>(length));
	}
}

int SpatialDisplacementLeafless::count(SurrogateTreeNode* tree)
{
	// Count all children plus ourselves (initial 1)
	int sum = 1;
	int maxDepth = 0;
	int childDepth = 0;
	for(vector<SurrogateTreeNode*>::iterator iter = tree->children->begin(); iter != tree->children->end(); ++iter)
	{
		sum += this->count(*iter);
		//childDepth = atoi((*iter)->data["count"].c_str());
		childDepth = atoi((*iter)->data[TreeNodeKey::DEPTH].c_str());
		if(childDepth > maxDepth)
		{
			maxDepth = childDepth;
		}
	}
	// Assign count to data
	tree->set(TreeNodeKey::SIZE, boost::lexical_cast<string>(sum));
	DiscursiveDebugPrint("Size of tree %s is %s in this and %d sublevels\n",tree->data[TreeNodeKey::NAME].c_str(),tree->data[TreeNodeKey::SIZE].c_str(),maxDepth);
	tree->set(TreeNodeKey::DEPTH, boost::lexical_cast<string>(maxDepth + 1));
	return sum;
}
