/*
 * git_repository_access.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef GIT_REPOSITORY_ACCESS_H_
#define GIT_REPOSITORY_ACCESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "repository_access.h"
#include "../model/surrogate_tree_node.h"

using namespace std;

class GitRepositoryAccess : public RepositoryAccess
{
private:
	string root;
	int generateLog();
	SurrogateTreeNode* generatePTree();
	void parseTimeBlock(SurrogateTreeNode* tree, long time, ifstream& log);
	void InsertByPathName(SurrogateTreeNode* tree, string pathname, long time);

public:
	GitRepositoryAccess(string repositoryRoot);
	SurrogateTreeNode* retrieve();
};

#endif /* GIT_REPOSITORY_ACCESS_H_ */
