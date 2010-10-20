/*
 * interactive_agent.cpp
 *
 *  Created on: Aug 19, 2010
 *      Author: programmer
 */


#include "./initial_agents.h"


RepositoryAccess*  interactive_agent()
{
	// Retrieve tree from GIT and create surrogate tree
	// for now, ask the user if local files or github project
	cout << "\n1==local git\n2==grit git remote Project\n3==any github project";
	cout << "\n4==http://hkit.googlecode.com/svn/trunk/\n5==any SVN url";
	cout << "\n6==pserver:anonymous@bnetd.cvs.sourceforge.net:/cvsroot/bnetd";
	cout << "\n7==any CVS url\n";
	cout << "\n8==any filesystem uri\n";
	int answer = 0;
	cin >> answer;
	RepositoryAccess* git;

	if(answer == 1)
	{
		git = new GitRepositoryAccess("~/Projects/source_tree_vis");
	}
	else if(answer == 2)
	{
		git = new GitHubRepositoryAccess("mojombo","grit");
	}
	else if(answer == 3)
	{
		// allow user to pick github user name/project
		char userNameBuffer[255];
		char projectNameBuffer[255];
		cout << "\nEnter the github user name\n";
		cin >> userNameBuffer;
		cout << "\nEnter the github project name\n";
		cin >> projectNameBuffer;

		git = new GitHubRepositoryAccess(userNameBuffer,projectNameBuffer);
	}
	else if(answer == 4)
	{
		git = new SvnRemoteRepositoryAccess(" http://hkit.googlecode.com/svn/trunk/");
	}
	else if(answer == 5)
	{
		char svnRepoBuffer[255];
		cout << "\nEnter a SVN url like http://hkit.googlecode.com/svn/trunk/\n";
		cin >> svnRepoBuffer;
		git = new SvnRemoteRepositoryAccess(svnRepoBuffer);
	}
	else if(answer == 6)
	{
		git = new CvsRemoteRepositoryAccess("pserver:anonymous@bnetd.cvs.sourceforge.net:/cvsroot/bnetd");
	}
	else if(answer == 7)
	{
		char cvsRepoBuffer[255];
		cout << "\nEnter a qualified CVS url like pserver:anonymous@bnetd.cvs.sourceforge.net:/cvsroot/bnetd\n";
		cin >> cvsRepoBuffer;
		git = new CvsRemoteRepositoryAccess(cvsRepoBuffer);
	}
	else if(answer == 8)
	{
		char fSRepoBuffer[255];
		cout << "\nEnter a qualified FS like /home/programmer/source_tree_vis/\n";
		cin >> fSRepoBuffer;
		git = new FileSystemRepositoryAccess(fSRepoBuffer);
	}
	return git;
}

// cli processor
RepositoryAccess* noninteractive_agent(int agentType, std::string agentString)
{
	RepositoryAccess* git;

	if(agentType == 1)
	{
		git = new GitRepositoryAccess(agentString);
	}
	else if(agentType == 2)
	{
		int colonPos = agentString.find(":");
		std::string gitUserName = agentString.substr(0,colonPos).c_str();
		std::string gitProjectName = agentString.substr(colonPos+1,agentString.size()).c_str();
		DiscursiveDebugPrint("agents,git,debug","testing \n|%s|,|%s|\n",gitUserName.c_str(),gitProjectName.c_str());

		git = new GitHubRepositoryAccess(gitUserName,gitProjectName);
	}
	else if(agentType == 3)
	{
		git = new SvnRemoteRepositoryAccess(agentString);
	}
	else if(agentType == 4)
	{
		git = new CvsRemoteRepositoryAccess(agentString);
	}
	else if(agentType == 6)
	{
		git = new FileSystemRepositoryAccess(agentString);
	}
	return git;
}


