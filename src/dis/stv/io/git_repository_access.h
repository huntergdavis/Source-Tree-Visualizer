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
#include "curl/curl.h"


class GitRepositoryAccess : public RepositoryAccess
{
private:
	string root;
	int generateLog(std::string *localGitLog);
	SurrogateTreeNode* generateTreeFromLog(std::string *buffer);
	void parseTimeBlock(SurrogateTreeNode* tree, long time, std::string *buffer);
	void InsertByPathName(SurrogateTreeNode* tree, std::string pathname, long fileTime);

public:
	GitRepositoryAccess(string repositoryRoot);
	SurrogateTreeNode* retrieve();
};

#endif /* GIT_REPOSITORY_ACCESS_H_ */
