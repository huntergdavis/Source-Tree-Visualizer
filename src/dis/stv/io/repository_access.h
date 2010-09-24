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

public:
	int snapshotJpgs;
	int globalInserts;
	int localInserts;
	int insertTarget;
	int jpgIndex;
	int scaleHeight;
	int scaleWidth;
	char* fileName;
	SurrogateTreeNode* source;
	int WriteJPGFromCanvas(Image* canvas);
	virtual SurrogateTreeNode* retrieve() = 0;
};

#endif /* REPOSITORY_ACCESS_H_ */
