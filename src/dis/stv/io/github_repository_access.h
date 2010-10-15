/*
 * git_repository_access.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef GITHUB_REPOSITORY_ACCESS_H_
#define GITHUB_REPOSITORY_ACCESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <time.h>
#include "repository_access.h"
#include "../model/surrogate_tree_node.h"
#include "curl/curl.h"
#include "../system/discursive_system.h"


using namespace std;

class GitHubRepositoryAccess : public RepositoryAccess
{
private:
	string root;
	string userNameCredentials;
	string repoNameCredentials;
	SurrogateTreeNode* generateTreeFromGitHub();
	void retrieveDetailedGitHubBlock(SurrogateTreeNode* treeResult,std::string *SHA1);
	void parseDetailedGitHubBlock(SurrogateTreeNode* treeResult,std::string *buffer);
	long parseDetailedGitHubFileBlock(std::string *buffer);
	long parseExactDateString(std::string *buffer);
	long retrieveDateFromGitHubFileName(std::string *gitHubFileName);
	static int writer(char *data, size_t size, size_t nmemb, std::string *buffer);

public:
	GitHubRepositoryAccess(string gitHubUserName,string gitHubProjectName);
	SurrogateTreeNode* retrieve();
};

#endif /* GITHUB_REPOSITORY_ACCESS_H_ */
