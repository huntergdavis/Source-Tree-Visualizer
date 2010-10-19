/*
 * git_repository_access.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#include "svn_remote_repository_access.h"

using namespace std;


// -------------------------------------------------------------------------
// API :: SvnRemoteRepositoryAccess::SvnRemoteRepositoryAccess
// PURPOSE :: instantiation of SvnRemoteRepositoryAccess
//         :: specifically, with a github url of project info
// PARAMETERS :: std::string gitHubUserName, std::string gitHubProjectName
// RETURN :: None
// -------------------------------------------------------------------------
SvnRemoteRepositoryAccess::SvnRemoteRepositoryAccess(std::string svnRemoteServerString)
{
	// make project root the yaml api info stream for svn
	this->remoteServerString = svnRemoteServerString;

	// add repo type of svn
	this->repoType = 3;


}


// -------------------------------------------------------------------------
// API :: SvnRemoteRepositoryAccess::generateTreeFromRemoteSvn
// PURPOSE :: generation function for tree when using svn as a medium
//         ::
// PARAMETERS ::
// RETURN :: SurrogateTreeNode* - containing tree values generated
// -------------------------------------------------------------------------
SurrogateTreeNode* SvnRemoteRepositoryAccess::generateTreeFromRemoteSvn()
{
	// sanity check
	if(this->repoType != 3)
	{
		// shouldn't get here
		// TODO: add error handling function with try-catch default
		DiscursiveError("Bad Repo Type");
	}

	// Blank tree
	SurrogateTreeNode* treeResult = new SurrogateTreeNode();
//	treeResult->data[TreeNodeKey::NAME] = TreeNodeKey::ROOT;

	// only generate the log in the first pass
	if(logGenerated == 0)
	{
		// don't re-generate logs if not necessary
		logGenerated = 1;

		// create a string for holding the svn log
		repoLog.reserve(10024);  // reserve 10k for fast allocation

		// create a string for holding the svn command
		std::string svnCommand = "svn log --verbose --non-interactive ";
		svnCommand += remoteServerString;

		// create a file pointer handle to our command output
		FILE *fp = popen(svnCommand.c_str(), "r" );

		// loop over all the file handle and put into stringstream
		while (true)
		{
		  int c = std::fgetc( fp );
		  if (c == EOF) break;
		  repoLog.push_back( (char)c );
		}
		std::fclose( fp );
		//DiscursiveDebugPrint("svn,repository access","SVN LOG RESULT %s",svnLog.c_str());
	}

    // generate tree entry from log file entry
    generateTreeFromLog(treeResult,&repoLog);

	// return the filled tree
	return treeResult;
}

// -------------------------------------------------------------------------
// API :: SvnRemoteRepositoryAccess::generateTreeFromLog
// PURPOSE :: generation function for ptree when using svn as a medium
//         ::
// PARAMETERS ::
// RETURN :: SurrogateTreeNode* - containing tree values generated
// -------------------------------------------------------------------------
void SvnRemoteRepositoryAccess::generateTreeFromLog(SurrogateTreeNode* tree,std::string *buffer)
{
	// Blank ptree
	//SurrogateTreeNode* result = new SurrogateTreeNode();
	tree->set(TreeNodeKey::NAME, TreeNodeKey::ROOT);
	tree->set(TreeNodeKey::REVISIONCREATED, localRevs);

	// For each time block, parse files and add to ptree
	char c,d;
	std::string str;
	for(int i = 0;i<(int)buffer->size()-1;i++)
	{
		c = buffer->at(i);
		d = buffer->at(i+1);
		if((c == '\n') && (d == '\n'))
		{
			parseTimeBlock(tree,&str);
			str.clear();
		}
		else
		{
			str += c;
		}
	}
	str.clear();
}
// -------------------------------------------------------------------------
// API :: SvnRemoteRepositoryAccess::parseTimeBlock
// PURPOSE :: parsing function for individual SVN commit log blocks
//         ::
// PARAMETERS ::
// RETURN :: SurrogateTreeNode* - containing tree values generated
// -------------------------------------------------------------------------
void SvnRemoteRepositoryAccess::parseTimeBlock(SurrogateTreeNode* tree, std::string *buffer)
{
	DiscursiveDebugPrint("svn,repository access","INDIVIDUAL TIME BLOCK %s\n",buffer->c_str());

	// at this point we have the individual SVN time block

	// let's pull out the date first, followed by any file additions
	std::string dateString;

	// the date is between the 2nd and 3rd pipe
	int secondPipeIndex;
	int firstPipeIndex;

	// find the first pipe
	firstPipeIndex = buffer->find("|");
	if(firstPipeIndex < 0)
	{
		//DiscursiveError("Bad Svn Pipe Parse");
		return;
	}

	// find the second pipe
	secondPipeIndex = buffer->find("|",firstPipeIndex+1);
	if(secondPipeIndex < 0)
	{
		//DiscursiveError("Bad Svn Pipe Parse");
		return;
	}

	// pull our date string from between index 2 and 3
	dateString = buffer->substr(secondPipeIndex+2,19);

	// print out the date
	//DiscursiveDebugPrint("svn,repository access","\nTHEDATE: |%s| index 1:%d 2:%d\n",dateString.c_str(),firstPipeIndex,secondPipeIndex);
	long dateEpoch = parseExactDateString(&dateString);


	// loop over each line and search for created files
	// created files are denoted by an 'A' tag
	// create an istringstream to parse the suboutput for added files
	std::istringstream svnTimeBlockSS(*buffer);

	// individual filename line storage
	std::string fileNameLine;

	// individual filename storage
	std::string fileNameString;

	// update either local revisions or global revisions
	if(revTarget > 0)
	{
		localRevs++;
	}
	else
	{
		globalRevs++;
		localRevs++;
		DiscursiveDebugPrint("svn,repository access","First Pass: Adding Revision Number %ld \nCurrent Tree Size %ld \n",globalRevs,currentTreeSize);
	}

	// loop over the detailed commit and find filenames
	int updateType = 0;
	while (getline (svnTimeBlockSS, fileNameLine))
	{
		if((fileNameLine.find("A") == 3) || (fileNameLine.find("U") == 3) || (fileNameLine.find("G") == 3))
		{
			updateType = 1;
		}
		if(fileNameLine.find("D") == 3)
		{
			updateType = 2;
		}

		if(updateType > 0)
		{
			// pull out the file name string
			fileNameString = fileNameLine.substr(5,fileNameLine.size()-5);
			//DiscursiveDebugPrint("svn,repository access","\nFILENAMESTRING: |%s|\n",fileNameString.c_str());

			// actually insert the file entry into the tree
			// increase the number of global inserts by one
			if((insertTarget > 0) && (localInserts < insertTarget))
			{
				localInserts++;
				if(updateType == 1)
				{
					InsertByPathName(tree,fileNameString,dateEpoch,1);
				}
				else
				{
					RemoveByPathName(tree,fileNameString);
				}
			}
			if((revTarget > 0) && (localRevs < revTarget))
			{
				if(updateType == 1)
				{
					InsertByPathName(tree,fileNameString,dateEpoch,1);
				}
				else
				{
					RemoveByPathName(tree,fileNameString);
				}
			}
			if((insertTarget == 0) && (revTarget == 0))
			{
				globalInserts++;
				if(updateType == 1)
				{
					InsertByPathName(tree,fileNameString,dateEpoch,1);
				}
				else
				{
					RemoveByPathName(tree,fileNameString);
				}
			}
			// reset update type
			updateType = 0;
		}
	}

}

// -------------------------------------------------------------------------
// API :: SvnRemoteRepositoryAccess::parseExactDateString
// PURPOSE :: returns date from svn log block
//         ::
// PARAMETERS :: std::string buffer - exact date of format
//            :: 2009-05-12 05:36:53
// RETURN :: long - oldest date associated with filename
// -------------------------------------------------------------------------
long SvnRemoteRepositoryAccess::parseExactDateString(std::string *buffer)
{
	// our time tm separated unix structure
	struct tm timeStructure;

	// our raw time structure
	time_t rawTime;

	// set our locale to US
	setlocale(LC_TIME, "en_US.iso88591");

	//strptime for the value
	//strptime("2007-10-09T23:18:20", "%Y-%m-%dT%H:%M:%S", &t);
	strptime(buffer->c_str(), "%Y-%m-%d %H:%M:%S", &timeStructure);

	rawTime = mktime(&timeStructure);
	//DiscursiveDebugPrint("svn,repository access","RAWTIME\n%ld\n",(long)rawTime);
	return (long) rawTime;
}


SurrogateTreeNode* SvnRemoteRepositoryAccess::retrieve()
{
	SurrogateTreeNode* result = NULL;

	if(this->repoType == 3)
	{
		result = this->generateTreeFromRemoteSvn();
	}

	return result;
}




