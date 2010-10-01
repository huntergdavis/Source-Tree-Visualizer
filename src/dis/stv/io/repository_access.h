/*
 * repository_access.h
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#ifndef REPOSITORY_ACCESS_H_
#define REPOSITORY_ACCESS_H_

#include "../model/surrogate_tree_node.h"
#include "../system/discursive_system.h"

#include <Magick++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace Magick;



class RepositoryAccess
{
protected:
	int repoType;
	std::string repoLog;

public:
	int snapshotJpgs;
	long globalInserts;
	long localInserts;
	long insertTarget;
	long revJpgs;
	long globalRevs;
	long localRevs;
	long revTarget;
	long jpgIndex;
	int scaleHeight;
	int scaleWidth;
	int logGenerated;
	int imageHeight;
	int imageWidth;
	long currentTreeSize;
	std::string fileName;
	SurrogateTreeNode* source;
	int WriteJPGFromCanvas(Image* canvas);
	virtual SurrogateTreeNode* retrieve() = 0;
};

#endif /* REPOSITORY_ACCESS_H_ */
