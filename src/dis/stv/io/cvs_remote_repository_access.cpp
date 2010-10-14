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
	treeResult->set(TreeNodeKey::NAME, TreeNodeKey::ROOT);
	treeResult->set(TreeNodeKey::REVISIONCREATED, localRevs);

	// only generate the log in the first pass
	if(logGenerated == 0)
	{
		// create a string for holding the cvs log
		repoLog.reserve(10024);  // reserve 10k for fast allocation

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
		  repoLog.push_back( (char)c );
		}
		std::fclose( fp );
		//DiscursiveDebugPrint("cvs,repository access","CVS LOG RESULT %s",cvsLog.c_str());

		// don't re-generate logs if not necessary
		logGenerated = 1;
	}
    // generate tree entry from log file entry
    generateTreeFromLog(treeResult,&repoLog);

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
	result->set(TreeNodeKey::NAME, TreeNodeKey::ROOT);
	result->set(TreeNodeKey::REVISIONCREATED, localRevs);

	// For each line, parse files and add to ptree
	// create an istringstream to parse the suboutput for added files
	std::istringstream cvsTimeBlockSS(*buffer);
	std::string str;

	// parse the cvs time block
	// loop over the detailed commit and find filenames
	while (getline (cvsTimeBlockSS, str))
	{
		if((str.find("A") == 0) || (str.find("U") == 0) || (str.find("G") == 0))
		{
			parseTimeBlock(tree,&str);
		}

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
			localRevs++;
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
		InsertByPathName(tree,fullyQualifiedName,dateEpoch,1);
	}
	if((revTarget > 0) && (localRevs < revTarget))
	{
		InsertByPathName(tree,fullyQualifiedName,dateEpoch,1);
	}
	if((insertTarget == 0) && (revTarget == 0))
	{
		globalInserts++;
		InsertByPathName(tree,fullyQualifiedName,dateEpoch,1);
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




