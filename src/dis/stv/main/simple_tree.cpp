/*
 * simple_tree.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */
#include <Magick++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../model/surrogate_tree_node.h"
#include "../dec/spatial_displacement.h"
#include "../draw/scanline_artist.h"
#include "../gen/space_colonizer.h"
#include "../system/discursive_system.h"
#include "./interactive_agent.h"

using namespace Magick;

// display our command usage
void display_usage( void )
{
	std::string usage_string = "\nsimple_tree [option] [optionstringorint] \n";
	usage_string += "where option can be any of the following";
	usage_string += "\n-g option - expects ~/Projects/source_tree_vis\n";
	usage_string += "\n-G option - expects gitHubusername:gitHubprojectname\n";
	usage_string += "\n-S option - expects http://hkit.googlecode.com/svn/trunk/\n";
	usage_string += "\n-C option - expects pserver:anonymous@bnetd.cvs.sourceforge.net:/cvsroot/bnetd\n";
	usage_string += "\n-d option - debug level, defaults to 1\n";

    printf("%s",usage_string.c_str());
    DiscursiveError("Copyright Discursive Labs LLC, 2010");
}


int main(int argc, char **argv)
{
	// set the debug level
	SetDiscursiveDebugLevel(0);


	// agentName equates to the passed command line parameter argument string
	char *agentName = NULL;

	// type of agent to use, ie command line options or interactive
	int agentType = 0;

	// our option string
	static const char *optString = "g:G:S:C:dh?";

	// loop over our command options in the normal unix way
	// reminder: g:G:S:C:dh?
	int opt = getopt( argc, argv, optString );
	    while( opt != -1 ) {
	        switch( opt ) {
	            case 'g':
	                localGit = optarg;
	                agentType = 1;
	                break;
	            case 'G':
	                remoteGitHub = optarg;
	                agentType = 2;
	                break;
	            case 'S':
	                svnName = optarg;
	                agentType = 3;
	                break;
	            case 'C':
	                cvsName = optarg;
	                agentType = 4;
	                break;
	            case 'd':
	            	// set the debug level
	            	SetDiscursiveDebugLevel(1);
	            	break;
	            case 'h':   /* fall-through is intentional */
	            case '?':
	                display_usage();
	                break;
	            default:
	                /* You won't actually get here. */
	            	DiscursiveError("an impossibility occured in error processing");
	                break;
	        }
	        // get the next Command Line option
	        opt = getopt( argc, argv, optString );
	    }


	// Tree image size parameters
	int WIDTH = 500;
	int HEIGHT = 500;

	// Tree Generation pipeline
	// create an abstract method for repository access
	RepositoryAccess* git;

	// let an interactive or command line agent create the repository access type
	switch(agentType)
	{
		case 0:
			git = interactive_agent();
			break;
		case 1:
			git = noninteractive_agent(agentType, )



	SurrogateTreeNode* source = git->retrieve();
	std::string sourceTreeNameOutput = "Source tree name is";
	sourceTreeNameOutput += source->data["name"].c_str();
	DebugPrint(sourceTreeNameOutput);

	// Decorate surrogate tree nodes with locations
	SpatialDisplacement* disp = new SpatialDisplacement(WIDTH,HEIGHT);
	disp->decorate(source);

	// Digitize decorated surrogate tree into line segment tree
	SpaceColonizer* digitizer = new SpaceColonizer(2);
	DrawableData* lines = digitizer->digitize(source);

	// Transform

	// Draw tree
	InitializeMagick(*argv);
	Image canvas(Geometry(WIDTH,HEIGHT),"white");
	ScanlineArtist* artist = new ScanlineArtist();
	artist->draw(canvas, lines);

	try
	{
		// Write the image to a file
		canvas.write( "out/tree.jpg" );
	}
	catch( Exception &error_ )
	{
		DiscursiveError(error_.what());
	}

	return 0;
}
