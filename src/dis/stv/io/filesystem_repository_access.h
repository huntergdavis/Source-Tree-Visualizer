/*
 * git_repository_access.h
 *
 *  Created on: Oct 20th, 2010
 *      Author: Hunter Davis
 */

#ifndef FILESYSTEM_REPOSITORY_ACCESS_H_
#define FILESYSTEM_REPOSITORY_ACCESS_H_

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
#include <sys/stat.h>



using namespace std;

class FileSystemRepositoryAccess : public RepositoryAccess
{
private:
	string root;
	string remoteServerString;
	int generateLog();
	int parseLog(SurrogateTreeNode* tree);
	long getFileCreationDate(std::string fileNameString);

public:
	FileSystemRepositoryAccess(std::string FileSystemString);
	SurrogateTreeNode* retrieve();
};

#endif /* FILESYSTEM_REPOSITORY_ACCESS_H_ */
