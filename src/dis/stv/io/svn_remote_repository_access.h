/*
 * git_repository_access.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef SVN_REMOTE_REPOSITORY_ACCESS_H_
#define SVN_REMOTE_REPOSITORY_ACCESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <time.h>
#include <stdio.h>
#include "repository_access.h"
#include "../model/surrogate_tree_node.h"
#include "curl/curl.h"


using namespace std;

class SvnRemoteRepositoryAccess : public RepositoryAccess
{
private:
	string root;
	string remoteServerString;
	SurrogateTreeNode* generateTreeFromRemoteSvn();
	void InsertByPathName(SurrogateTreeNode* tree, string pathname, long time);
	void retrieveDetailedGitHubBlock(SurrogateTreeNode* treeResult,std::string *SHA1);
	void parseDetailedGitHubBlock(SurrogateTreeNode* treeResult,std::string *buffer);
	long parseDetailedGitHubFileBlock(std::string *buffer);
	long parseExactDateString(std::string *buffer);
	long retrieveDateFromGitHubFileName(std::string *gitHubFileName);

public:
	SvnRemoteRepositoryAccess(std::string svnRemoteServerString);
	SurrogateTreeNode* retrieve();
};

#endif /* SVN_REMOTE_REPOSITORY_ACCESS_H_ */
