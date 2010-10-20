/*
 * git_repository_access.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#include "filesystem_repository_access.h"

using namespace std;


// -------------------------------------------------------------------------
// API :: FileSystemRepositoryAccess::FileSystemRepositoryAccess
// PURPOSE :: instantiation of FileSystemRepositoryAccess
//         :: specifically, with a filesystem url of project info
// PARAMETERS :: std::string FileSystemRepositoryAccess
// RETURN :: None
// -------------------------------------------------------------------------
FileSystemRepositoryAccess::FileSystemRepositoryAccess(std::string FileSystemServerString)
{
	// make project root the yaml api info stream for svn
	this->remoteServerString = FileSystemServerString;

	// add repo type of filesystem
	this->repoType = 6;

}

// -------------------------------------------------------------------------
// API :: FileSystemRepositoryAccess::retrieve
// PURPOSE :: retrieves log and generates surrogate tree
//         ::
// PARAMETERS ::
// RETURN :: SurrogateTreeNode - the generated surrogate tree
// -------------------------------------------------------------------------
SurrogateTreeNode* FileSystemRepositoryAccess::retrieve()
{
	SurrogateTreeNode* result = NULL;


	if(this->repoType != 6)
	{
		return result;
	}

	// Blank ptree
	result = new SurrogateTreeNode();
	result->set(TreeNodeKey::NAME, TreeNodeKey::ROOT);

	// generate log
	generateLog();

	// parse log
	parseLog(result);

	return result;
};

// -------------------------------------------------------------------------
// API :: FileSystemRepositoryAccess::generateLog
// PURPOSE :: generates log file to parse
//         ::
// PARAMETERS ::
// RETURN :: int - status variable
// -------------------------------------------------------------------------
int FileSystemRepositoryAccess::generateLog()
{
	// only generate the log in the first pass
	if(logGenerated == 0)
	{
		// create a string for holding the svn log
		repoLog.reserve(10024);  // reserve 10k for fast allocation

		// create a string for holding the svn command
		std::string localFileCommand = "find " + this->remoteServerString;

		// create a file pointer handle to our command output
		FILE *fp = popen(localFileCommand.c_str(), "r" );

		// loop over all the file handle and put into string
		while (true)
		{
		  int c = std::fgetc( fp );
		  if (c == EOF) break;
		  repoLog.push_back( (char)c );
		}
		std::fclose( fp );

		// show the log (debug)
		DiscursiveDebugPrint("filesystem,repository access",repoLog.c_str());

		// don't re-generate logs if not necessary
		logGenerated = 1;
	}
    // figure out a meaningful return code
    return 1;
};

// -------------------------------------------------------------------------
// API :: FileSystemRepositoryAccess::parseLog
// PURPOSE :: parses log file a line at a time
//         ::
// PARAMETERS :: SurrogateTreeNode* tree - current tree
// RETURN :: int - status variable
// -------------------------------------------------------------------------
int FileSystemRepositoryAccess::parseLog(SurrogateTreeNode* tree)
{

	// stringstream with our log
	std::istringstream localFileSystemLogSS(repoLog);

	// individual filename line storage
	std::string fileNameLine;

	// previous filename storage for SVN lexical issues
	std::string previousNameString = "";

	// update either local revisions or global revisions
	if(revTarget > 0)
	{
		localRevs++;
	}
	else
	{
		globalRevs++;
		localRevs++;
		DiscursiveDebugPrint("filesystem,repository access","First Pass: Adding Revision Number %ld \n",globalRevs);
	}

	// loop over the detailed commit and find new filenames
	while (getline (localFileSystemLogSS, fileNameLine))
	{
		// ignore the root . directory
		if((fileNameLine != ".") && (fileNameLine != " \n"))
		{
			//fileNameLine = fileNameLine.substr(2,fileNameLine.size()-2);

			// clear out any intro slashes
			//if(fileNameLine.find("/") == 0)
			//{
			//	fileNameLine = fileNameLine.substr(1,fileNameLine.size()-1);
			//}


			// we need to check if the previous name string fully contained in our string
			// if so, it's a directory and should be removed before we add the new items
			// this clears out the empty 'file' directory
			if(previousNameString != "")
			{
				if(fileNameLine.find(previousNameString.c_str()) != std::string::npos)
				{
					RemoveByPathName(tree,previousNameString);
				}
			}

			// find the creation date for the file
			long dateEpoch = getFileCreationDate("/" + fileNameLine);

			if((insertTarget > 0) && (localInserts < insertTarget))
			{
				localInserts++;
				InsertByPathName(tree,fileNameLine,dateEpoch,1);
			}
			if((revTarget > 0) && (localRevs < revTarget))
			{
				InsertByPathName(tree,fileNameLine,dateEpoch,1);
			}
			if((insertTarget == 0) && (revTarget == 0))
			{
				globalInserts++;
				InsertByPathName(tree,fileNameLine,dateEpoch,1);
			}

			previousNameString = fileNameLine + "/";
		}
	}
	return 1;
};

// -------------------------------------------------------------------------
// API :: FileSystemRepositoryAccess::parseLog
// PURPOSE :: returns long of file creation date
//         ::
// PARAMETERS ::
// RETURN :: long - file creation date
// -------------------------------------------------------------------------
long FileSystemRepositoryAccess::getFileCreationDate(std::string fileNameString)
{
	long ourFileCreationDate = 0;
	struct stat stFileInfo;
	int fileDoesntExist;

	// Attempt to get the file attributes
	fileDoesntExist = stat(fileNameString.c_str(),&stFileInfo);

	// if the file is found, return the creation/modification date
	if(fileDoesntExist == 0)
	{
	  ourFileCreationDate = stFileInfo.st_mtime;
	}
	else
	{
	  DiscursiveError("Cannot stat file %s\n",fileNameString.c_str());
	}

	// return the date
	return ourFileCreationDate;

};

