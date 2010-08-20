/*
 * interactive_agent.h
 *
 *  Created on: Aug 19, 2010
 *      Author: Hunter Davis
 */

#ifndef INITIAL_AGENTS_H_
#define INITIAL_AGENTS_H_

#include "../system/discursive_system.h"
#include "../io/git_repository_access.h"
#include "../io/github_repository_access.h"
#include "../io/svn_remote_repository_access.h"
#include "../io/cvs_remote_repository_access.h"


RepositoryAccess* interactive_agent();
RepositoryAccess* noninteractive_agent(int agentType, std::string agentString);

#endif /* INTERACTIVE_AGENT_H_ */
