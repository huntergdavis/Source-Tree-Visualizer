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
#include "../system/discursive_system.h"



using namespace std;

class SvnRemoteRepositoryAccess : public RepositoryAccess
{
private:
	string root;
	string remoteServerString;
	SurrogateTreeNode* generateTreeFromRemoteSvn();
	void InsertByPathName(SurrogateTreeNode* tree, string pathname, long time);
	long parseExactDateString(std::string *buffer);
	void generateTreeFromLog(SurrogateTreeNode* tree,std::string *buffer);
	void parseTimeBlock(SurrogateTreeNode* tree, std::string *buffer);


public:
	SvnRemoteRepositoryAccess(std::string svnRemoteServerString);
	SurrogateTreeNode* retrieve();
};

#endif /* SVN_REMOTE_REPOSITORY_ACCESS_H_ */
