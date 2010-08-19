/*
 * git_repository_access.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#include "git_repository_access.h"

using namespace std;

using boost::property_tree::ptree;

const string TEMP_FILE = ".gitlog.tmp";
// ISO 8601 timestamp
//const string GIT_COMMAND = "git whatchanged --pretty=format:%ci --reverse --raw --encoding=UTF-8";
// Unix timestamp
const string GIT_COMMAND = "git whatchanged --pretty=format:%ct --reverse --raw --encoding=UTF-8";

// -------------------------------------------------------------------------
// API :: GitRepositoryAccess::GitRepositoryAccess
// PURPOSE :: instantiation of GitRepositoryAccess
//         :: specifically, with a local root of project info
// PARAMETERS :: std::string repositoryRoot
// RETURN :: None
// -------------------------------------------------------------------------
GitRepositoryAccess::GitRepositoryAccess(string repositoryRoot)
{
	this->root = repositoryRoot;

	// add git repo type of local files
	this->repoType = 1;

}

void GitRepositoryAccess::InsertByPathName(SurrogateTreeNode* tree, string pathname, long time)
{
	// Split off first part of path
	int firstIndex = pathname.find("/");
	if(firstIndex == 0)
	{
		// Our path started with a slash but we already have a root
		// So remove first slash and ignore
		pathname = pathname.substr(1,pathname.length()-1);
		firstIndex = pathname.find("/");
	}
	if(firstIndex == -1)
	{
		// We have no more path left.  Just a single entry (leaf)
		SurrogateTreeNode* file = new SurrogateTreeNode();
		string timeStr = boost::lexical_cast<string>(time);
		file->data["creation_time"] = timeStr;
		file->data["name"] = pathname;
		printf("Adding node '%s' @ time %ld\n",pathname.c_str(),time);
		tree->children.push_back(file);
	}
	else
	{
		// Split first string out for this nodes name
		string name = pathname.substr(0,firstIndex);
		// Look for node in children
		SurrogateTreeNode* node = NULL;

		vector<SurrogateTreeNode*>::iterator iter = tree->children.begin();
		for(;iter != tree->children.end(); ++iter)
		{
			SurrogateTreeNode* local = *iter;
			string nameComp = local->data["name"];
			//printf("Comparing %s to %s\n",nameComp.c_str(),name.c_str());
			if(!nameComp.compare(name))
			{
				// Found node
				node = (*iter);
				// Update node time if necessary
				if(time < atol(node->data["creation_time"].c_str()))
				{
					printf("Updating time of node[\"%s\"] to %ld from %ld\n", name.c_str(), time, atol(node->data["creation_time"].c_str()));
					node->data["creation_time"] = boost::lexical_cast<string>(time);
				}
				break;
			}
		}
		// If child not found, add new node
		if(node == NULL)
		{
			node = new SurrogateTreeNode();
			string timeStr = boost::lexical_cast<string>(time);
			node->data["creation_time"] = timeStr;
			node->data["name"] = name;
			printf("Adding node '%s' @ time %ld\n",name.c_str(),time);
			tree->children.push_back(node);
		}
		// Else, use found node

		// Parse rest of string
		this->InsertByPathName(node, pathname.substr(firstIndex+1,(pathname.length() - firstIndex - 1)),time);
	}
}

void GitRepositoryAccess::parseTimeBlock(SurrogateTreeNode* tree, long time, ifstream& log)
{
	printf("Parsing block for time %ld\n",time);
	// Parse each line
	char c;
	string str;
	while(log.get(c))
	{
		if(c != '\n')
		{
			str += c;
		}
		else
		{
			if(str.length() == 0)
			{
				// Finished
				break;
			}
			else
			{
				// Examine line for file name and Git operation
				vector<string> cols;
				boost::split(cols, str, boost::is_any_of("\t "));
				// Vector should be at least 2 columns.  Last two are: <Git Op> <Filename with path>
				if(cols.size() > 1)
				{
					vector<string>::reverse_iterator iter = cols.rbegin();
					string filename = *iter;
					++iter;
					string op = *iter;
					//printf("Filename: '%s', Op: %s\n",filename.c_str(),op.c_str());
					if(!op.compare("A"))
					{
						printf("Inserting %s @ %ld\n",filename.c_str(),time);
						InsertByPathName(tree,filename,time);
					}
				}
				str.clear();
			}
		}
	}
}

SurrogateTreeNode* GitRepositoryAccess::generatePTreeFromLog()
{
	// Blank ptree
	SurrogateTreeNode* result = new SurrogateTreeNode();
	result->data["name"] = "root";
	// Load log file
	ifstream log;
	log.open( TEMP_FILE.c_str(), ios::in );
	if(log)
	{
		// For each time block, parse files and add to ptree
		char c;
		string str;
		while(log.get(c))
		{
			if(c != '\n')
			{
				str += c;
			}
			else
			{
				long time = atol(str.c_str());
				parseTimeBlock(result,time,log);
				str.clear();
			}
		}
		log.close();
	}

	return result;
}

int GitRepositoryAccess::generateLog()
{
	int commandReturnValue = 0;

	if(this->gitRepoType == 1)
	{
		string gitLogWithOutput = "cd " + this->root + "; " + GIT_COMMAND + " > " + TEMP_FILE;
		printf("Log command: %s\n",gitLogWithOutput.c_str());
		commandReturnValue = system(gitLogWithOutput.c_str());
	}

	// return a value
	return commandReturnValue;
}

SurrogateTreeNode* GitRepositoryAccess::retrieve()
{
	SurrogateTreeNode* result = NULL;

	if(this->gitRepoType == 1)
	{
		// If we generated a log file
		if(!this->generateLog())
		{
			// Create the ptree from log
			result = this->generatePTreeFromLog();
			printf("Generated tree with name '%s'\n", result->data["name"].c_str());
		}
	}

	return result;
}



