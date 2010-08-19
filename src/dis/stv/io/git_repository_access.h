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

using namespace std;

class GitRepositoryAccess : public RepositoryAccess
{
private:
	string root;
	string userNameCredentials;
	string repoNameCredentials;
	int gitRepoType;	// 1=local,2=github
	int generateLog();
	SurrogateTreeNode* generatePTreeFromLog();
	SurrogateTreeNode* generatePTreeFromGitHub();
	void parseTimeBlock(SurrogateTreeNode* tree, long time, ifstream& log);
	void InsertByPathName(SurrogateTreeNode* tree, string pathname, long time);
	void retrieveDetailedGitHubBlock(SurrogateTreeNode* treeResult,std::string *SHA1);
	void parseDetailedGitHubBlock(SurrogateTreeNode* treeResult,std::string *);
	static int writer(char *data, size_t size, size_t nmemb, std::string *buffer);

public:
	GitRepositoryAccess(string repositoryRoot);
	GitRepositoryAccess(string gitHubUserName,string gitHubProjectName);
	SurrogateTreeNode* retrieve();
};

#endif /* GIT_REPOSITORY_ACCESS_H_ */
