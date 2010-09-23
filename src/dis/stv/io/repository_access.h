/*
 * repository_access.h
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#ifndef REPOSITORY_ACCESS_H_
#define REPOSITORY_ACCESS_H_

#include <boost/property_tree/ptree.hpp>
#include "../model/surrogate_tree_node.h"
#include "../system/discursive_system.h"


class RepositoryAccess
{
protected:
	int repoType;

public:
	int snapshotJpgs;
	int jpgIndex;
	int scaleHeight;
	int scaleWidth;
	SurrogateTreeNode* source;
	int GenerateTreeAndWriteJPG(std::string *fileName);
	virtual SurrogateTreeNode* retrieve() = 0;
};

#endif /* REPOSITORY_ACCESS_H_ */
