/*
 * git_repository_access.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#include "git_repository_access.h"

using namespace std;

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

void GitRepositoryAccess::InsertByPathName(SurrogateTreeNode* tree, string pathname, long fileTime)
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
		string timeStr = boost::lexical_cast<string>(fileTime);
		file->set(TreeNodeKey::CREATION_TIME, timeStr);
		file->set(TreeNodeKey::NAME, pathname);
		file->set(TreeNodeKey::REVISIONCREATED, localRevs);
		DiscursiveDebugPrint("git,repository access","Adding node '%s' @ time %ld\n",pathname.c_str(),fileTime);
		tree->children->push_back(file);
	}
	else
	{
		// Split first string out for this nodes name
		string name = pathname.substr(0,firstIndex);
		// Look for node in children
		SurrogateTreeNode* node = NULL;

		vector<SurrogateTreeNode*>::iterator iter = tree->children->begin();
		for(;iter != tree->children->end(); ++iter)
		{
			SurrogateTreeNode* local = *iter;
			string nameComp = local->data[TreeNodeKey::NAME];
			//printf("Comparing %s to %s\n",nameComp.c_str(),name.c_str());
			if(!nameComp.compare(name))
			{
				// Found node
				node = (*iter);
				// Update node time if necessary
				if(fileTime < atol(node->data[TreeNodeKey::CREATION_TIME].c_str()))
				{
					DiscursiveDebugPrint("git,repository access","Updating time of node[\"%s\"] to %ld from %ld\n", name.c_str(), fileTime, atol(node->data[TreeNodeKey::CREATION_TIME].c_str()));
					node->set(TreeNodeKey::CREATION_TIME, boost::lexical_cast<string>(fileTime));
					node->set(TreeNodeKey::REVISIONCREATED, localRevs);
				}
				break;
			}
		}
		// If child not found, add new node
		if(node == NULL)
		{
			node = new SurrogateTreeNode();
			string timeStr = boost::lexical_cast<string>(fileTime);
			node->set(TreeNodeKey::CREATION_TIME, timeStr);
			node->set(TreeNodeKey::NAME, name);
			node->set(TreeNodeKey::REVISIONCREATED, localRevs);
			DiscursiveDebugPrint("git,repository access","Adding node '%s' @ time %ld\n",name.c_str(),fileTime);
			tree->children->push_back(node);
		}
		// Else, use found node
		this->InsertByPathName(node, pathname.substr(firstIndex+1,(pathname.length() - firstIndex - 1)),fileTime);
	}
}

void GitRepositoryAccess::parseTimeBlock(SurrogateTreeNode* tree, long time, std::string *buffer)
{
	//DiscursiveDebugPrint("git,repository access","Parsing block for time %ld\n BLOCK IS: \n\n%s\n\n",time,buffer->c_str());

	// loop over each line and search for created files
	// created files are denoted by an '... A' tag sequence
	// create an istringstream to parse the suboutput for added files
	std::istringstream localGitBlockSS(*buffer);

	// individual filename line storage
	std::string fileNameLine;

	// individual filename storage
	std::string fileNameString;

	// ..a identifier location storage
	// as it will be used in search, init to -1
	int aLocation = -1;

	// loop over the detailed commit and find new filenames
	while (getline (localGitBlockSS, fileNameLine))
	{
		// test for new commit blocks, update revision number
		if(fileNameLine.find(":") != 0)
		{
			if(revTarget > 0)
			{
				localRevs++;
			}
			else
			{
				globalRevs++;
			}
		}

		aLocation = fileNameLine.find("... A");
		if(aLocation > 0)
		{
			// locate the filename in git string
			fileNameString = fileNameLine.substr(aLocation+6,fileNameLine.size()-aLocation+6);

			// increase the number of global inserts by one
			if((insertTarget > 0) && (localInserts < insertTarget))
			{
					localInserts++;
					InsertByPathName(tree,fileNameString,time);
			}
			if((revTarget > 0) && (localRevs < revTarget))
			{
				InsertByPathName(tree,fileNameString,time);
			}
			if((insertTarget == 0) && (revTarget == 0))
			{
				globalInserts++;
				InsertByPathName(tree,fileNameString,time);
			}

		}
	}

}

SurrogateTreeNode* GitRepositoryAccess::generateTreeFromLog(std::string *buffer)
{
	// Blank ptree
	SurrogateTreeNode* result = new SurrogateTreeNode();
	result->set(TreeNodeKey::NAME, TreeNodeKey::ROOT);

	// For each time block, parse files and add to ptree
	char c;
	string str;
	for(int i=0;i<(int)buffer->size();i++)
	{
		c = buffer->at(i);
		if(c != '\n')
		{
			str += c;
		}
		else
		{
			long fileTime = atol(str.c_str());
			parseTimeBlock(result,fileTime,&str);
			str.clear();
		}
	}

	return result;
}

int GitRepositoryAccess::generateLog()
{
	// only generate the log in the first pass
	if(logGenerated == 0)
	{
		// create a string for holding the svn log
		repoLog.reserve(10024);  // reserve 10k for fast allocation

		// create a string for holding the svn command
		std::string localGitCommand = "cd " + this->root + "; " + GIT_COMMAND;

		// create a file pointer handle to our command output
		FILE *fp = popen(localGitCommand.c_str(), "r" );

		// loop over all the file handle and put into string
		while (true)
		{
		  int c = std::fgetc( fp );
		  if (c == EOF) break;
		  repoLog.push_back( (char)c );
		}
		std::fclose( fp );

		// show the log (debug)
		DiscursiveDebugPrint("git,repository access",repoLog.c_str());

		// don't re-generate logs if not necessary
		logGenerated = 1;
	}
    // figure out a meaningful return code
    return 1;
}

SurrogateTreeNode* GitRepositoryAccess::retrieve()
{
	SurrogateTreeNode* result = NULL;

	if(this->repoType == 1)
	{
		std::string localGitFile;
		// If we generated a log file
		if(this->generateLog())
		{
			// Create the tree from log
			result = this->generateTreeFromLog(&repoLog);
			DiscursiveDebugPrint("git,repository access","Generated tree with name '%s'\n", result->data[TreeNodeKey::NAME].c_str());
		}
	}

	return result;
}



