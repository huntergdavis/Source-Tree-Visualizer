/*
 * spatial_displacement.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#include "spatial_displacement.h"
#include "../system/discursive_system.h"


using namespace std;

SpatialDisplacement::SpatialDisplacement(int width, int height):width(width), height(height)
{
	scaleWidth = 0.9;
	scaleHeight = 0.85;
}

SpatialDisplacement::SpatialDisplacement(int width, int height, double scaleWidth, double scaleHeight):width(width), height(height) ,scaleWidth(scaleWidth), scaleHeight(scaleHeight)
{

}

void SpatialDisplacement::decorate(SurrogateTreeNode* tree)
{
	// Iterate over each node with children
	// First, "count".  Calculates child center of mass and subtree depth
	this->count(tree);
	// Second, float weighted surrogate nodes into position
	//this->expand(tree,(3.14159/2),0,0);
	// Max tree depth
	double maxDepth = tree->findMax("depth");
	this->expand2(tree,(3.14159/2),0,0,(this->height)/maxDepth);
	// Third, transform coordinates
	this->transform(tree);
}


// Scale point values to fit within width and height give
// with root at (width/2, height)
// Transformation flips y values and shifts x values
// after scaling
void SpatialDisplacement::transform(SurrogateTreeNode* tree)
{
	// Calculate resize scaling factors
	double allowedWidth = scaleWidth*this->width;
	double allowedHeight = scaleHeight*this->height;
	double xMax = tree->findMax("x");
	double xMin = tree->findMin("x");
	double yMax = tree->findMax("y");
	double yMin = tree->findMin("y");
	DiscursiveDebugPrint("Mins: (%f,%f), Maxs: (%f,%f)\n",xMin,yMin,xMax,yMax);
	double currWidth = xMax - xMin;
	double currHeight = yMax - yMin;

	double minDim = min(allowedHeight, allowedWidth);

	double scalingFactorW = minDim/currWidth;
	double scalingFactorH = minDim/currHeight;

	// Transform points to look more "naturally tree-like"
	tree->scale("y", scalingFactorH);
	PropertyInverter inverter(this->height * 0.98);
	tree->transform("y",&inverter);

	PropertyShifter shifter(-1*((xMax + xMin) / 2));
	tree->transform("x",&shifter);
	// Scale tree values
	tree->scale("x", scalingFactorW);
	PropertyShifter shifter2(minDim * (2 - scaleWidth) / 2);
	tree->transform("x",&shifter2);
}

// Sorted in increasing order
void SpatialDisplacement::insertOrderedBy(vector<SurrogateTreeNode*>* list, SurrogateTreeNode* tree, string property)
{
	//printf("Inserting '%s' from %s.  Current size: %d\n",property.c_str(),tree->data["name"].c_str(), list->size());
	SurrogateTreeNode* node;
	bool inserted = false;
	for(vector<SurrogateTreeNode*>::iterator iter = list->begin(); iter != list->end(); ++iter)
	{
		node = *iter;
		//printf("%ld < %ld?\n",atol(tree->data[property].c_str()),atol(node->data[property].c_str()));
		if(atol(tree->data[property].c_str()) < atol(node->data[property].c_str()))
		{
			list->insert(iter,tree);
			inserted = true;
			break;
		}
	}
	if(!inserted)
	{
		//printf("Inserting at end\n");
		list->push_back(tree);
	}
}

void SpatialDisplacement::expand(SurrogateTreeNode* tree, double rootAngle, double rootX, double rootY)
{
	if(tree->children.size() > 0)
	{
		// Determine initial layout based on creation time of child nodes.
		// Files are split into different list than directories.
		vector<SurrogateTreeNode*> files;
		vector<SurrogateTreeNode*> dirs;
		SurrogateTreeNode* node = NULL;
		int maxChild = 1;
		int childSize;
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
		{
			node = *iter;
			// Directory
			if(node->children.size() > 0)
			{
				//printf("Inserting into dirs...\n");
				this->insertOrderedBy(&dirs,node,"creation_time");
				childSize = atoi(node->data["size"].c_str());
				if(childSize > maxChild )
				{
					maxChild = childSize;
				}
			}
			// File
			else
			{
				//printf("Inserting into files...\n");
				this->insertOrderedBy(&files,node,"creation_time");
			}
		}

		// Map for retrieving resutls
		unordered_map<SurrogateTreeNode*,TreeDisplacementNode*> simPairs;
		int gap = 10;
		// If total is even, use initial offset
		int offset = 0;
		if((dirs.size() + files.size()) % 2 == 0)
		{
			offset = -gap/2;
		}

		// Layout nodes.  Order from middle outward by creation time (oldest in the middle)
		// Use minDiff version of constructor
		TimeSteppedPhysicsEngine* engine = new TimeSteppedPhysicsEngine(0.001,0.1);
		// Add centered attractor mass first
		double attractorWeight = -0.75 * maxChild;
		if(attractorWeight > -1)
		{
			attractorWeight = -1;
		}
		engine->addMass(new FixedAttractor(attractorWeight,0,0));

		// Layout directories first
		bool left = true;
		int dist = 0;
		TreeDisplacementNode* treeNode;
		int i = 0;
		for(; i < (int)dirs.size(); i++)
		{
			dist = (i+1)/2;
			if(left)
			{
				treeNode = new TreeDisplacementNode(atoi(dirs[i]->data["size"].c_str()),offset-dist*gap);
			}
			else
			{
				treeNode = new TreeDisplacementNode(atoi(dirs[i]->data["size"].c_str()),offset+dist*gap);
			}
			//printf("Adding node '%s' at initial position (%f,%f)\n", dirs[i]->data["name"].c_str(),treeNode->getX(), treeNode->getY());
			simPairs[dirs[i]] = treeNode;
			engine->addMass(treeNode);
			left = !left;
		}
		// Now layout files
		// 1 branch/node per LEAVES_PER_NODE files
		int LEAVES_PER_NODE = 10;
		int lastLeaf = -LEAVES_PER_NODE;
		for(int j = 0; j < (int)files.size(); j++)
		{
			if(j - lastLeaf >= LEAVES_PER_NODE)
			{
				lastLeaf = j;
				dist = ((i + j + 1)/2);
				if(left)
				{
					treeNode = new TreeDisplacementNode(atoi(files[j]->data["size"].c_str()),offset-dist*gap);
				}
				else
				{
					treeNode = new TreeDisplacementNode(atoi(files[j]->data["size"].c_str()),offset+dist*gap);
				}
				//printf("Adding node '%s' at initial position (%f,%f)\n", files[j]->data["name"].c_str(),treeNode->getX(), treeNode->getY());
				simPairs[files[j]] = treeNode;
				engine->addMass(treeNode);
				left = !left;
			}
		}
		// Run simulation
		engine->run();
		// Normalize positions to range [0,pi]
		double rangeMin = 3.14159/4;
		double rangeMax = 3*3.14159/4;
		double least = DBL_MAX;
		double most = -DBL_MAX;
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
		{
			node = *iter;
			treeNode = simPairs[node];
			if(treeNode->getX() < least)
			{
				least = treeNode->getX();
			}
			if(treeNode->getX() > most)
			{
				most = treeNode->getX();
			}
		}
		double trans = 0;
		// This only happens when we have a single node
		if(most == least)
		{
			// Adjust
			trans = ((rangeMax - rangeMin)/2);
			// Only valid because we know we have a single node when we get here
			least = treeNode->getX() - 1;
		}
		else
		{
			trans = (rangeMax - rangeMin)/(most - least);
		}
		if(most - least != 0)
		{
			for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
			{
				node = *iter;
				treeNode = simPairs[node];
				treeNode->setRotation((treeNode->getX() - least)*trans + rangeMin + rootAngle - (3.14159/2));
				//printf("%s adjusted to (%f,%f)\n",node->data["name"].c_str(),treeNode->getX(),treeNode->getY());
			}
		}
		else
		{
			DiscursiveDebugPrint("Serious Error.  (Max: %f) Most: %f, Least: %f\n", DBL_MAX, most, least);
			DiscursiveError("Serious Error");
		}
		// Transform positions to arc
		double arcRadius = 10.0;
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
		{
			node = *iter;
			treeNode = simPairs[node];
			treeNode->setLocation(arcRadius * cos(treeNode->getRotation()),arcRadius * sin(treeNode->getRotation()));
			//printf("%s transformed to (%f,%f)\n",node->data["name"].c_str(),treeNode->getX(),treeNode->getY());
		}

		// Update new positions
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
		{
			node = *iter;
			treeNode = simPairs[node];
			double newX = treeNode->getX() + rootX;
			double newY = treeNode->getY() + rootY;
			//printf("%s final position at (%f,%f)\n",node->data["name"].c_str(),newX, newY);
			node->data["x"] = boost::lexical_cast<string>(newX);
			node->data["y"] = boost::lexical_cast<string>(newY);
			//printf("%s @ (%s,%s)\n",node->data["name"].c_str(),node->data["x"].c_str(),node->data["y"].c_str());
			// Run expand on child
			double childRot = treeNode->getRotation() + ((3.14159/2)-treeNode->getRotation())/2;
			this->expand(node,childRot,newX,newY);
		}
	}
	else
	{
		//printf("No children for node '%s'\n", tree->data["name"].c_str());
	}
}

void SpatialDisplacement::expand2(SurrogateTreeNode* tree, double rootAngle, double rootX, double rootY, double allowedHeight)
{
	if(tree->children.size() > 0)
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
		double minChildMass = DBL_MAX;
		for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
		{
			children++;
			node = *iter;
			// Directory
			if(node->children.size() > 0)
			{
				//printf("Inserting into dirs...\n");
				this->insertOrderedBy(&dirs,node,"size");
				childSize = atoi(node->data["size"].c_str());
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
				//printf("Inserting into files...\n");
				this->insertOrderedBy(&files,node,"creation_time");
				mass++;
				if(minChildMass > 1)
				{
					minChildMass = 1;
				}
			}
		}

		// Map for retrieving resutls
		unordered_map<SurrogateTreeNode*,int> pairs;
		double masses[children];
		double positions[children];
		memset(positions,0,sizeof(positions));
		// Layout directories first
		// Start left if even number of items.
		bool left = children % 2 != 0;
		int center = children / 2;
		int dist = 0;
		//TreeDisplacementNode* treeNode;
		int i = 0;
		int location;
		int nodeMass;
		for(; i < (int)dirs.size(); i++)
		{
			dist = (i+1)/2;
			nodeMass = atoi(dirs[i]->data["size"].c_str());
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
		for(int j = 0; j < (int)files.size(); j++)
		{
			dist = ((i + j + 1)/2);
			nodeMass = 1;
			if(left)
			{
				location = center - dist;
			}
			else
			{
				location = center + dist;
			}
			masses[location] = nodeMass;
			pairs[files[j]] = location;
			for(int k = location; k < children; k++)
			{
				positions[k] += nodeMass;
			}
			left = !left;
		}

		// Calculate spacing to range [0,splay]
		double deltaSplay = 0;
		double splay = 3.14159 / 2;
		double com;
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

		positions[0] = 0;
		com = 0;
		for(i = 1; i < children; i++)
		{
			positions[i] = (positions[i] - minChildMass)*deltaSplay;
			com += (positions[i] * masses[i]);
		}
		// Final part of CoM calculation
		com /= mass;

		// Transform positions to arc
		//double arcRadius = 10.0;
//		double deltaAngle = splay / children;
		int maxDepth = (int)tree->findMax("depth");
		int depth;
		double arcRadius = allowedHeight/maxDepth;
		double ratio;
		double angle;

		// Controls width of fan-out.  > 1 : Wide tree
		//							   < 1 : Narrow tree
		double widthHeightScaleFactor = 1.0;
		// Transform positions to arc and Update new positions
		// Dirs first
		for(i = 0; i < (int)dirs.size(); i++)
		{
			node = dirs[i];
			depth = atoi(node->data["depth"].c_str());
			ratio = depth / (double)maxDepth;
			angle = rootAngle - (positions[pairs[node]] - com);
			double newX = rootX + (ratio * arcRadius * cos(angle));
			double newY = rootY + (ratio * arcRadius * widthHeightScaleFactor * sin(angle));
			node->data["x"] = boost::lexical_cast<string>(newX);
			node->data["y"] = boost::lexical_cast<string>(newY);
			// Run expand on child
			double childRot = (3.14159/2);//angle;// + ((3.14159/2)-angle)/2;
			this->expand2(node,childRot,newX,newY,allowedHeight - arcRadius);
		}
		// Then files
		for(int j = 0; j < (int)files.size(); j++)
		{
			node = files[j];
			depth = atoi(node->data["depth"].c_str());
			ratio = depth / (double)maxDepth;
			angle = rootAngle - (positions[pairs[node]] - com);
			double newX = rootX + (ratio * arcRadius * cos(angle));
			double newY = rootY + (ratio * arcRadius * widthHeightScaleFactor * sin(angle));
			node->data["x"] = boost::lexical_cast<string>(newX);
			node->data["y"] = boost::lexical_cast<string>(newY);
		}
//		for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
//		{
//			node = *iter;
//			depth = atoi(node->data["depth"].c_str());
//			ratio = depth / (double)maxDepth;
//			angle = rootAngle + ();
//			double newX = rootX + ();
//			double newY = rootY;
//			//printf("%s final position at (%f,%f)\n",node->data["name"].c_str(),newX, newY);
//			node->data["x"] = boost::lexical_cast<string>(newX);
//			node->data["y"] = boost::lexical_cast<string>(newY);
//			printf("%s @ (%s,%s)\n",node->data["name"].c_str(),node->data["x"].c_str(),node->data["y"].c_str());
//			// Run expand on child
//			double childRot = treeNode->getRotation() + ((3.14159/2)-treeNode->getRotation())/2;
//			this->expand(node,childRot,newX,newY,allowedHeight - arcRadius);
//		}
	}
	else
	{
		//printf("No children for node '%s'\n", tree->data["name"].c_str());
	}
}

int SpatialDisplacement::count(SurrogateTreeNode* tree)
{
	// Count all children plus ourselves (initial 1)
	int sum = 1;
	int maxDepth = 0;
	int childDepth = 0;
	for(vector<SurrogateTreeNode*>::iterator iter = tree->children.begin(); iter != tree->children.end(); ++iter)
	{
		sum += this->count(*iter);
		//childDepth = atoi((*iter)->data["count"].c_str());
		childDepth = atoi((*iter)->data["depth"].c_str());
		if(childDepth > maxDepth)
		{
			maxDepth = childDepth;
		}
	}
	// Assign count to data
	tree->data["size"] = boost::lexical_cast<string>(sum);
	DiscursiveDebugPrint("Size of tree %s is %s in this and %d sublevels\n",tree->data["name"].c_str(),tree->data["size"].c_str(),maxDepth);
	tree->data["depth"] = boost::lexical_cast<string>(maxDepth + 1);
	return sum;
}
