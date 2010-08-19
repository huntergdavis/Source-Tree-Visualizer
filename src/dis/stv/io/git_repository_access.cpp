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
	this->gitRepoType = 1;

	// username and repo credentials
	this->userNameCredentials = "";
	this->repoNameCredentials = root;
}

// -------------------------------------------------------------------------
// API :: GitRepositoryAccess::GitRepositoryAccess
// PURPOSE :: instantiation of GitRepositoryAccess
//         :: specifically, with a github url of project info
// PARAMETERS :: std::string gitHubUserName, std::string gitHubProjectName
// RETURN :: None
// -------------------------------------------------------------------------
GitRepositoryAccess::GitRepositoryAccess(std::string gitHubUserName,std::string gitHubProjectName)
{
	// make project root the yaml api info stream for github
    std::stringstream projectRootStream;
    //http://github.com/api/v2/yaml/repos/show/schacon/grit
	projectRootStream << "http://github.com/api/v2/yaml/repos/show/";
	projectRootStream << gitHubUserName;
	projectRootStream << "/";
	projectRootStream << gitHubProjectName;
	this->root = projectRootStream.str();

	// add username and project name credentials to data structure
	this->userNameCredentials = gitHubUserName;
	this->repoNameCredentials = gitHubProjectName;

	// debug output for user credentials
	printf("\nGitHub User Name: %s, GitHub Project Name: %s\n",gitHubUserName.c_str(),gitHubProjectName.c_str());

	// add git repo type of github
	this->gitRepoType = 2;


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

// -------------------------------------------------------------------------
// API :: GitRepositoryAccess::generatePTreeFromGitHub
// PURPOSE :: generation function for ptree when using github as a medium
//         ::
// PARAMETERS ::
// RETURN :: SurrogateTreeNode* - containing tree values generated
// -------------------------------------------------------------------------
SurrogateTreeNode* GitRepositoryAccess::generatePTreeFromGitHub()
{
	// sanity check
	if(this->gitRepoType != 2)
	{
		// shouldn't get here
		// TODO: add error handling function with try-catch default
		exit(1);
	}

	// Blank ptree
	SurrogateTreeNode* treeResult = new SurrogateTreeNode();
	treeResult->data["name"] = "root";

	// first retrieve the base log file from which to retrieve sha1 hashes for commit blobs
	std::string topLevelApiUrl;
	topLevelApiUrl = "http://github.com/api/v2/yaml/commits/list/";
	topLevelApiUrl += userNameCredentials;
	topLevelApiUrl += "/";
	topLevelApiUrl += repoNameCredentials;
	topLevelApiUrl += "/";
	topLevelApiUrl += "master";

	// Write any errors in here
	static char errorBuffer[CURL_ERROR_SIZE];

	// Write all expected data in here
	static string buffer;

	// set up a lib curl instantiation
	CURL *curl;
	CURLcode result;

	// Create our curl handle
	curl = curl_easy_init();

	if (curl)
	{
		// Now set up all of the curl options
	   curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
	   curl_easy_setopt(curl, CURLOPT_URL, topLevelApiUrl.c_str());
	   curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	   curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	   curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

	   // Attempt to retrieve the remote page
	   printf("\n curl is attempting to pull %s\n",topLevelApiUrl.c_str());
	   result = curl_easy_perform(curl);

	   // Always cleanup
	   curl_easy_cleanup(curl);

	   // Did we succeed?
	   if (result == CURLE_OK)
	   {
		   // create an instringstream object to linify the input ala terminal
		   std::istringstream topLevelSS(buffer);

		   // loop over the outer string stream object and search for sha1 keys
		   // keep track of line numbering
		   int linenum = 0;
		   // our top level line storage
		   std::string topLevelLine;
		   // sha1 storage
		   std::string topLevelSHA1;
		   while (getline (topLevelSS, topLevelLine))
		   {
			   linenum++;

			   // the "id:" identifier will be 2 for entries, 4 for parents
			   int idVal = topLevelLine.find("id:");
			   // if we have a entry that's not a parent entry
			   if(idVal == 2)
			   {
				   topLevelSHA1 = topLevelLine.substr(6,topLevelLine.size()-5);
				   //printf("\n sha1 %s\n",SHA1->c_str());
				   retrieveDetailedGitHubBlock(&topLevelSHA1,treeResult);
			   }
		   }
	   }
	}
	// return the filled tree
	return treeResult;
}

// -------------------------------------------------------------------------
// API :: GitRepositoryAccess::retrieveDetailedGitHubBlock
// PURPOSE :: retrieves detailed git hub info page for a sha1 key
//         ::
// PARAMETERS :: std::string *SHA1 - sha1 key
//            :: std::string SurrogateTreeNode* treeResult - resultant tree
// RETURN :: SurrogateTreeNode* - containing tree values generated
// -------------------------------------------------------------------------
void GitRepositoryAccess::retrieveDetailedGitHubBlock(std::string *SHA1,SurrogateTreeNode* treeResult)
{
	// At this point we have the SHA1 key, now lets pull the filename and creation time

	// Write any errors in here
	static char errorBuffer[CURL_ERROR_SIZE];

	// Write all expected data in here
	static string buffer;

	// set up a lib curl instantiation
	CURL *curl;
	CURLcode result;

	// create this custom github api sha1 URL with strings
	std::string gitHubApiUrl = "http://github.com/api/v2/yaml/commits/show/";
	gitHubApiUrl += userNameCredentials;
	gitHubApiUrl += "/";
	gitHubApiUrl += repoNameCredentials;
	gitHubApiUrl += "/";
	gitHubApiUrl += SHA1->c_str();

	// Create our curl handle for the detailed info for the SHA key
	curl = curl_easy_init();

	if (curl)
	{
		// Now set up all of the curl options
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
		curl_easy_setopt(curl, CURLOPT_URL, gitHubApiUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_HEADER, 0);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

		// clear out the buffer to be sure
		buffer = "";

		// Attempt to retrieve the remote page
		printf("\n curl is attempting to pull %s\n",gitHubApiUrl.c_str());
		result = curl_easy_perform(curl);

		// Always cleanup
		curl_easy_cleanup(curl);

		// Did we succeed?
		if (result == CURLE_OK)
		{
		   printf("DDDDDDDDDDDDDDDDD%s",buffer.c_str());
		   parseDetailedGitHubBlock(&buffer,treeResult);
		}
	}
}

// -------------------------------------------------------------------------
// API :: GitRepositoryAccess::parseDetailedGitHubBlock
// PURPOSE :: parses detailed git hub info page
//         ::
// PARAMETERS :: std::string *SHA1 - sha1 key
//            :: std::string SurrogateTreeNode* treeResult - resultant tree
// RETURN :: SurrogateTreeNode* - containing tree values generated
// -------------------------------------------------------------------------
void GitRepositoryAccess::parseDetailedGitHubBlock(std::string *buffer,SurrogateTreeNode* treeResult)
{
	// create an istringstream to parse the suboutput
	std::istringstream gitHubBlockSS(*buffer);

	// each item does contain the time, but it's stringified
	// TODO: add a time string parser using STRFTIME (not hard)
	// TODO: pull out times with filenames using time first tagging
	// TODO: time first tagging = collect time, apply it as "time" of commit till next, loop
	long huntersBirthday = 357603262;

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
	else if(this->gitRepoType == 2)
	{
		result = this->generatePTreeFromGitHub();
	}

	return result;
}

// -------------------------------------------------------------------------
// API :: GitRepositoryAccess::writer
// PURPOSE :: callback function for curl to write buffers to
//         ::
// PARAMETERS :: char *data, size_t size, size_t nmemb, std::string *buffer
// RETURN :: None
// -------------------------------------------------------------------------
int GitRepositoryAccess::writer(char *data, size_t size, size_t nmemb, std::string *buffer)
{
   // What we will return
   int result = 0;

   // Is there anything in the buffer?
   if (buffer != NULL)
   {
     // Append the data to the buffer
     buffer->append(data, size * nmemb);

     // How much did we write?
     result = size * nmemb;
   }
   return result;
}


