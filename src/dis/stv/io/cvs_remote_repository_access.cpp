/*
 * git_repository_access.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#include "cvs_remote_repository_access.h"

using namespace std;


// -------------------------------------------------------------------------
// API :: CvsRemoteRepositoryAccess::CvsRemoteRepositoryAccess
// PURPOSE :: instantiation of CvsRemoteRepositoryAccess
//         :: specifically, with a github url of project info
// PARAMETERS :: std::string gitHubUserName, std::string gitHubProjectName
// RETURN :: None
// -------------------------------------------------------------------------
CvsRemoteRepositoryAccess::CvsRemoteRepositoryAccess(std::string cvsRemoteServerString)
{
	// make project root the yaml api info stream for cvs
	this->remoteServerString = cvsRemoteServerString;

	// add repo type of cvs
	this->repoType = 4;

	// blank the last parsed date
	lastDate = 0;

}

void CvsRemoteRepositoryAccess::InsertByPathName(SurrogateTreeNode* tree, string pathname, long time)
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
		//printf("Adding node '%s' @ time %ld\n",pathname.c_str(),time);
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
					//printf("Updating time of node[\"%s\"] to %ld from %ld\n", name.c_str(), time, atol(node->data["creation_time"].c_str()));
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
			//printf("Adding node '%s' @ time %ld\n",name.c_str(),time);
			tree->children.push_back(node);
		}
		// Else, use found node

		// Parse rest of string
		this->InsertByPathName(node, pathname.substr(firstIndex+1,(pathname.length() - firstIndex - 1)),time);
	}
}


// -------------------------------------------------------------------------
// API :: CvsRemoteRepositoryAccess::generateTreeFromRemoteCvs
// PURPOSE :: generation function for tree when using cvs as a medium
//         ::
// PARAMETERS ::
// RETURN :: SurrogateTreeNode* - containing tree values generated
// -------------------------------------------------------------------------
SurrogateTreeNode* CvsRemoteRepositoryAccess::generateTreeFromRemoteCvs()
{
	// sanity check
	if(this->repoType != 4)
	{
		// shouldn't get here
		// TODO: add error handling function with try-catch default
		DiscursiveError("bad repo type");
	}

	// Blank tree
	SurrogateTreeNode* treeResult = new SurrogateTreeNode();
	treeResult->data["name"] = "root";

	// create a string for holding the cvs log
	std::string cvsLog;
	cvsLog.reserve(10024);  // reserve 10k for fast allocation

	// create a string for holding the cvs command
	std::string cvsCommand = " cvs -d:";
	cvsCommand += remoteServerString;
	cvsCommand += " history -a -xGA";

	// create a file pointer handle to our command output
	FILE *fp = popen(cvsCommand.c_str(), "r" );

	// loop over all the file handle and put into string
    while (true)
    {
      int c = std::fgetc( fp );
      if (c == EOF) break;
      cvsLog.push_back( (char)c );
    }
    std::fclose( fp );
    //DiscursiveDebugPrint("CVS LOG RESULT %s",cvsLog.c_str());

    // generate tree entry from log file entry
    generateTreeFromLog(treeResult,&cvsLog);

	// return the filled tree
	return treeResult;
}

// -------------------------------------------------------------------------
// API :: CvsRemoteRepositoryAccess::generateTreeFromLog
// PURPOSE :: generation function for ptree when using cvs as a medium
//         ::
// PARAMETERS ::
// RETURN :: SurrogateTreeNode* - containing tree values generated
// -------------------------------------------------------------------------
void CvsRemoteRepositoryAccess::generateTreeFromLog(SurrogateTreeNode* tree,std::string *buffer)
{
	// Blank ptree
	SurrogateTreeNode* result = new SurrogateTreeNode();
	result->data["name"] = "root";

	// For each line, parse files and add to ptree
	// create an istringstream to parse the suboutput for added files
	std::istringstream cvsTimeBlockSS(*buffer);
	std::string str;

	// parse the cvs time block
	// loop over the detailed commit and find filenames
	while (getline (cvsTimeBlockSS, str))
	{
		parseTimeBlock(tree,&str);
	}

}
// -------------------------------------------------------------------------
// API :: CvsRemoteRepositoryAccess::parseTimeBlock
// PURPOSE :: parsing function for individual SVN commit log blocks
//         ::
// PARAMETERS ::
// RETURN :: SurrogateTreeNode* - containing tree values generated
// -------------------------------------------------------------------------
void CvsRemoteRepositoryAccess::parseTimeBlock(SurrogateTreeNode* tree, std::string *buffer)
{
	//printf("\nINDIVIDUAL TIME BLOCK |%s|\n",buffer->c_str());

	// at this point we have the individual CVS time block
	// let's pull out the date first, followed by any file additions
	std::string dateString;

	// pull our date string from set index
	dateString = buffer->substr(2,16);

	// print out the date
	//printf("\nTHEDATE: |%s| \n",dateString.c_str());
	long dateEpoch = parseExactDateString(&dateString);


	// new date = new revision
	if(dateEpoch != lastDate)
	{
		// update either local revions or global revisions
		if(revTarget > 0)
		{
			localRevs++;
		}
		else
		{
			globalRevs++;
		}

		// set last date
		lastDate = dateEpoch;

	}

	// store all the spacing tabstops from the line
	int tabStops[8];
	int numberTabStops = 0;
	for(int i = 0;i<(int)buffer->size()-1;i++)
	{
		if(buffer->at(i) == ' ')
		{
			if(buffer->at(i+1) != ' ')
			{
				tabStops[numberTabStops] = i;
				numberTabStops++;
				//printf("\nTAB STOP #%d, VALUE %d\n",numberTabStops,i);
			}
		}
	}

	// now pull the filename (unqualified -- no directory)
	std::string unqualifiedFileName;
	int fileNameWhiteSpaceLocation = buffer->find(" ",tabStops[5]+1);
	unqualifiedFileName = buffer->substr(tabStops[5]+1,fileNameWhiteSpaceLocation-tabStops[5]-1);
	//printf("\nUNQUALIFIEDFILENAME: |%s|\n",unqualifiedFileName.c_str());

	// now pull the directory
	std::string directoryName;
	int directoryNameWhiteSpaceLocation = buffer->find(" ",tabStops[6]+1);
	directoryName = buffer->substr(tabStops[6]+1,directoryNameWhiteSpaceLocation-tabStops[6]-1);
	//printf("\nDIRECTORYNAME: |%s|\n",directoryName.c_str());

	// now put the directory and filename together
	std::string fullyQualifiedName;

	// remove any directory ./ qualifier
	if(directoryName.find("./") == 0)
	{
		fullyQualifiedName = directoryName.substr(2,directoryName.length()-2);
	}
	else
	{
		fullyQualifiedName = directoryName;
	}

	// add trailing slash to directory
	fullyQualifiedName += "/";

	// add filename to fully qualified name
	fullyQualifiedName += unqualifiedFileName;
	//printf("\nQUALIFIEDNAME: |%s|\n",fullyQualifiedName.c_str());


	// actually insert path into tree
	// increase the number of global inserts by one
	if((insertTarget > 0) && (localInserts < insertTarget))
	{
		localInserts++;
		InsertByPathName(tree,fullyQualifiedName,dateEpoch);
	}
	if((revTarget > 0) && (localRevs < revTarget))
	{
		InsertByPathName(tree,fullyQualifiedName,dateEpoch);
	}
	if((insertTarget == 0) && (revTarget == 0))
	{
		globalInserts++;
		InsertByPathName(tree,fullyQualifiedName,dateEpoch);
	}

}

// -------------------------------------------------------------------------
// API :: CvsRemoteRepositoryAccess::parseExactDateString
// PURPOSE :: returns date from cvs log block
//         ::
// PARAMETERS :: std::string buffer - exact date of format
//            :: 2009-05-12 05:36
// RETURN :: long - oldest date associated with filename
// -------------------------------------------------------------------------
long CvsRemoteRepositoryAccess::parseExactDateString(std::string *buffer)
{
	// our time tm separated unix structure
	struct tm timeStructure;

	// our raw time structure
	time_t rawTime;

	// set our locale to US
	setlocale(LC_TIME, "en_US.iso88591");

	//strptime for the value
	//strptime("2007-10-09T23:18:20", "%Y-%m-%dT%H:%M:%S", &t);
	strptime(buffer->c_str(), "%Y-%m-%d %H:%M", &timeStructure);

	rawTime = mktime(&timeStructure);
	//printf("RAWTIME\n%ld\n",(long)rawTime);
	return (long) rawTime;
}


SurrogateTreeNode* CvsRemoteRepositoryAccess::retrieve()
{
	SurrogateTreeNode* result = NULL;

	if(this->repoType == 4)
	{
		result = this->generateTreeFromRemoteCvs();
	}

	return result;
}




