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

void SvnRemoteRepositoryAccess::InsertByPathName(SurrogateTreeNode* tree, string pathname, long time)
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
		DebugPrint("Adding node '%s' @ time %ld\n",pathname.c_str(),time);
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
			//DebugPrint("Comparing %s to %s\n",nameComp.c_str(),name.c_str());
			if(!nameComp.compare(name))
			{
				// Found node
				node = (*iter);
				// Update node time if necessary
				if(time < atol(node->data["creation_time"].c_str()))
				{
					DebugPrint("Updating time of node[\"%s\"] to %ld from %ld\n", name.c_str(), time, atol(node->data["creation_time"].c_str()));
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
			DebugPrint("Adding node '%s' @ time %ld\n",name.c_str(),time);
			tree->children.push_back(node);
		}
		// Else, use found node

		// Parse rest of string
		this->InsertByPathName(node, pathname.substr(firstIndex+1,(pathname.length() - firstIndex - 1)),time);
	}
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
	treeResult->data["name"] = "root";

	// create a string for holding the svn log
	std::string svnLog;
	svnLog.reserve(10024);  // reserve 10k for fast allocation

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
      svnLog.push_back( (char)c );
    }
    std::fclose( fp );
    //DebugPrint("SVN LOG RESULT %s",svnLog.c_str());

    // generate tree entry from log file entry
    generateTreeFromLog(treeResult,&svnLog);

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
	SurrogateTreeNode* result = new SurrogateTreeNode();
	result->data["name"] = "root";

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
		}
		else
		{
			str += c;
		}
	}
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
	//DebugPrint("INDIVIDUAL TIME BLOCK %s",buffer->c_str());

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
		DiscursiveError("Bad Svn Pipe Parse");
	}

	// find the second pipe
	secondPipeIndex = buffer->find("|",firstPipeIndex+1);
	if(secondPipeIndex < 0)
	{
		DiscursiveError("Bad Svn Pipe Parse");
	}

	// pull our date string from between index 2 and 3
	dateString = buffer->substr(secondPipeIndex+2,19);

	// print out the date
	//DebugPrint("\nTHEDATE: |%s| index 1:%d 2:%d\n",dateString.c_str(),firstPipeIndex,secondPipeIndex);
	long dateEpoch = parseExactDateString(&dateString);


	// loop over each line and search for created files
	// created files are denoted by an 'A' tag
	// create an istringstream to parse the suboutput for added files
	std::istringstream svnTimeBlockSS(*buffer);

	// individual filename line storage
	std::string fileNameLine;

	// individual filename storage
	std::string fileNameString;

	// loop over the detailed commit and find filenames
	while (getline (svnTimeBlockSS, fileNameLine))
	{
		if(fileNameLine.find("A") == 3)
		{
			fileNameString = fileNameLine.substr(5,fileNameLine.size()-5);
			//DebugPrint("\nFILENAMESTRING: |%s|\n",fileNameString.c_str());
			// actually insert the file entry into the tree
			InsertByPathName(tree,fileNameString,dateEpoch);
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
	//DebugPrint("RAWTIME\n%ld\n",(long)rawTime);
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




