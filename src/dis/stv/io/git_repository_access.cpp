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
				localRevs++;
				globalRevs++;
			}
		}

		//M,A,R,C,U = GIT ADD/MODIFY statuses
		//D = delete status
		int gitStatus = 0;
		aLocation = fileNameLine.find("... M");
		if (aLocation <= 0)
		{
			aLocation = fileNameLine.find("... A");
			if(aLocation > 0)
			{
				gitStatus = 1;
			}
		}
		if (aLocation <= 0)
		{
			aLocation = fileNameLine.find("... R");
			if(aLocation > 0)
			{
				gitStatus = 1;
			}
		}
		if (aLocation <= 0)
		{
			aLocation = fileNameLine.find("... C");
			if(aLocation > 0)
			{
				gitStatus = 1;
			}
		}
		if (aLocation <= 0)
		{
			aLocation = fileNameLine.find("... U");
			if(aLocation > 0)
			{
				gitStatus = 1;
			}
		}
		if (aLocation <= 0)
		{
			aLocation = fileNameLine.find("... D");
			if(aLocation > 0)
			{
				gitStatus = 2;
			}
		}
		if(aLocation > 0)
		{
			// locate the filename in git string
			fileNameString = fileNameLine.substr(aLocation+6,fileNameLine.size()-aLocation+6);

			// increase the number of global inserts by one
			if((insertTarget > 0) && (localInserts < insertTarget))
			{
					localInserts++;
					if(gitStatus == 1)
					{
						InsertByPathName(tree,fileNameString,time,1);
					}
					else if(gitStatus == 2)
					{
						RemoveByPathName(tree,fileNameString);
					}
			}
			if((revTarget > 0) && (localRevs < revTarget))
			{
				if(gitStatus == 1)
				{
					InsertByPathName(tree,fileNameString,time,1);
				}
				else if(gitStatus == 2)
				{
					RemoveByPathName(tree,fileNameString);
				}
			}
			if((insertTarget == 0) && (revTarget == 0))
			{
				globalInserts++;
				if(gitStatus == 1)
				{
					InsertByPathName(tree,fileNameString,time,1);
				}
				else if(gitStatus == 2)
				{
					RemoveByPathName(tree,fileNameString);
				}
			}
		}

	}

}

SurrogateTreeNode* GitRepositoryAccess::generateTreeFromLog(std::string *buffer)
{
	// Blank ptree
	SurrogateTreeNode* result = new SurrogateTreeNode();
	result->set(TreeNodeKey::NAME, TreeNodeKey::ROOT);
	DiscursiveDebugPrint("git","LOG: %s\n",buffer);

	// store most recent file time
	long fileTime = 0;

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
			// file time strings are 10 in length
			if(str.length() == 10)
			{
				fileTime = atol(str.c_str());
			}
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



