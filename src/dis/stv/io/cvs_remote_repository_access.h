/*
 * git_repository_access.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef CVS_REMOTE_REPOSITORY_ACCESS_H_
#define CVS_REMOTE_REPOSITORY_ACCESS_H_

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
#include "../system/discursive_system.h"
#include "curl/curl.h"


using namespace std;

class CvsRemoteRepositoryAccess : public RepositoryAccess
{
private:
	long lastDate;
	string root;
	string remoteServerString;
	SurrogateTreeNode* generateTreeFromRemoteCvs();
	long parseExactDateString(std::string *buffer);
	void generateTreeFromLog(SurrogateTreeNode* tree,std::string *buffer);
	void parseTimeBlock(SurrogateTreeNode* tree, std::string *buffer, int delItem);


public:
	CvsRemoteRepositoryAccess(std::string cvsRemoteServerString);
	SurrogateTreeNode* retrieve();
};

#endif /* CVS_REMOTE_REPOSITORY_ACCESS_H_ */
