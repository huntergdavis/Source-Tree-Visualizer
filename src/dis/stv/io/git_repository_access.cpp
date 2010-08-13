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

GitRepositoryAccess::GitRepositoryAccess(string repositoryRoot)
{
	this->root = repositoryRoot;
}

void GitRepositoryAccess::parseTimeBlock(ptree* tree, long time, ifstream& log)
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
				printf("Row had %d cols\n", cols.size());
				if(cols.size() > 1)
				{
					vector<string>::reverse_iterator iter = cols.rbegin();
					string filename = *iter;
					++iter;
					string op = *iter;
					printf("Filename: '%s', Op: %s\n",filename.c_str(),op.c_str());
					if(!op.compare("A"))
					{
						printf("Inserting %s @ %ld\n",filename.c_str(),time);
						tree->put(filename,time);
					}
				}
				str.clear();
			}
		}
	}
}

ptree* GitRepositoryAccess::generatePTree()
{
	// Blank ptree
	ptree* result = new ptree();
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
	string gitLogWithOutput = "cd " + this->root + "; " + GIT_COMMAND + " > " + TEMP_FILE;
    printf("Log command: %s\n",gitLogWithOutput.c_str());
	commandReturnValue = system(gitLogWithOutput.c_str());
	return commandReturnValue;
}

ptree* GitRepositoryAccess::retrieve()
{
	ptree* result = NULL;

	// If we generated a log file
	if(!this->generateLog())
	{
		// Create the ptree from log
		result = this->generatePTree();
	}

	return result;
}
