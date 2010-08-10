#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

// git.cpp - source file for functions related to git tree parsing and pulling
std::string gitLogCommand = "git log "
    "--pretty=format:user:%aN%n%ct "
    "--reverse --raw --encoding=UTF-8 "
    "--no-renames";
 

int getCustomGitLog(std::string directory,std::string outfile)
{
	printf("\nmaking an outfile %s \n",outfile.c_str());
	int commandReturnValue = 0;
	std::string gitLogWithOutput = gitLogCommand + " > " + outfile;
	commandReturnValue = system(gitLogCommand.c_str());
	return commandReturnValue;
}

