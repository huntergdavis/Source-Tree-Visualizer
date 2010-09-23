/*
 * simple_tree.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#include "../system/discursive_system.h"
#include "./initial_agents.h"

#include "repository_access.h"
#include <Magick++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../model/surrogate_tree_node.h"
#include "../dec/spatial_displacement.h"
#include "../draw/scanline_artist.h"
#include "../gen/space_colonizer.h"

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
	usage_string += "\n-i option - interactive mode (asks you questions) \n";
	usage_string += "\n-d option - debug level, defaults to 1\n";
	usage_string += "\n-O option - output file name, defaults to tree.jpg\n";
	usage_string += "\n-m option - output many many .jpgs in sequence\n";
	usage_string += "\n-s option - start number for many jpg tree rendering, default is 3\n";
	usage_string += "\n-f option - finish number for many jpg tree rendering, default is treesize\n";
	usage_string += "\n-t option - step value for many jpg tree rendering, default is 1\n";
	usage_string += "\n-W option - spatial displacement scaling width level, defaults to .9\n";
	usage_string += "\n-H option - spatial displacement scaling height level, defaults to .85\n";


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

	// scaling factors to use for spatial displacement
	double scaleHeight = 0.85;
	double scaleWidth = 0.9;

	// our filename for output file
	std::string fileName = "tree.jpg";

	// should we make many jpgs?
	int manyJpgs = 0;

	// options for many jpgs
	int jpgStep = 1;
	int jpgStart = 3;
	int jpgStop = 10000;

	// our option string
	static const char *optString = "g:G:S:C:H:W:O:s:f:t:midh?";

	// loop over our command options in the normal unix way

	int opt;
	opt = getopt( argc, argv, optString );
	while( opt != -1 ) {
		switch( opt ) {
			case 'g':
				agentName = optarg;
				agentType = 1;
				break;
			case 'G':
				agentName = optarg;
				agentType = 2;
				break;
			case 'S':
				agentName = optarg;
				agentType = 3;
				break;
			case 'C':
				agentName = optarg;
				agentType = 4;
				break;
			case 'W':
				scaleWidth = atof(optarg);
				break;
			case 'm':
				manyJpgs = 1;
				break;
			case 'H':
				scaleHeight = atof(optarg);
				break;
			case 's':
				jpgStart = atoi(optarg);
				break;
			case 'f':
				jpgStop = atoi(optarg);
				break;
			case 't':
				jpgStep = atoi(optarg);
				break;
			case 'O':
				fileName = optarg;
				break;
			case 'd':
				// set the debug level
				SetDiscursiveDebugLevel(1);
				break;
			case 'i':
				// set the interactivity level
				agentType = 5;
				break;
			case 'h':   /* fall-through is intentional */
			case '?':
				display_usage();
				break;
			default:
				DiscursiveError("bad cli, bad!");
				break;
		}
		// get the next Command Line option
		opt = getopt( argc, argv, optString );
	}


	// Tree Generation pipeline
	// create an abstract method for repository access
	RepositoryAccess* git;

	// let an interactive or command line agent create the repository access type
	switch(agentType)
	{
		case 5:
			git = interactive_agent();
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			git = noninteractive_agent(agentType, agentName);
			break;
		case 0:
			display_usage();	
			break;	
        default:
            /* You won't actually get here. */
        	DiscursiveError("an impossibility occured in error processing");
            break;
	}


    // set our repository single or manyjpg options
	git->snapshotJpgs = manyJpgs;
	git->jpgIndex = 1000;
	git->scaleHeight = scaleHeight;
	git->scaleWidth = scaleWidth;
	git->fileName = (char*)fileName.c_str();
	git->globalInserts = 0;
	git->localInserts = 0;
	git->insertTarget = 0;

	// retrieve our source tree
	git->source = git->retrieve();

	// loop over a bunch of increasingly large trees
	if(manyJpgs && (git->globalInserts > 2))
	{
		if (jpgStop >= git->globalInserts)
		{
			jpgStop = git->globalInserts-1;
		}
		// use our user-set parameters to define our step
		for(int i = jpgStart; i<jpgStop;i+= jpgStep)
		{
			// reset our insert variables
			git->localInserts = 0;
			git->insertTarget = i;

			// retrieve our source tree
			git->source = git->retrieve();

			// init libmagick
			//InitializeMagick("/");
			std::string sourceTreeNameOutput = "Source tree name is";
			sourceTreeNameOutput += git->source->data["name"].c_str();
			DiscursiveDebugPrint(sourceTreeNameOutput);

			// Decorate surrogate tree nodes with locations
			SpatialDisplacement* disp = new SpatialDisplacement(500,500,scaleWidth,scaleHeight);
			disp->decorate(git->source);

			// Digitize decorated surrogate tree into line segment tree
			SpaceColonizer* digitizer = new SpaceColonizer(2);
			DrawableData* lines = digitizer->digitize(git->source);

			// Transform

			// Draw tree
			Image canvas(Geometry(500,500),"white");
			ScanlineArtist* artist = new ScanlineArtist();
			artist->draw(canvas, lines);

			// actually generate a tree (or the final tree if many)
			git->WriteJPGFromCanvas(&canvas);

		}
	}
	// init libmagick
	//InitializeMagick("/");
	std::string sourceTreeNameOutput = "Source tree name is";
	sourceTreeNameOutput += git->source->data["name"].c_str();
	DiscursiveDebugPrint(sourceTreeNameOutput);

	// Decorate surrogate tree nodes with locations
	SpatialDisplacement* disp = new SpatialDisplacement(500,500,scaleWidth,scaleHeight);
	disp->decorate(git->source);

	// Digitize decorated surrogate tree into line segment tree
	SpaceColonizer* digitizer = new SpaceColonizer(2);
	DrawableData* lines = digitizer->digitize(git->source);

	// Transform

	// Draw tree
	Image canvas(Geometry(500,500),"white");
	ScanlineArtist* artist = new ScanlineArtist();
	artist->draw(canvas, lines);

	// actually generate a tree (or the final tree if many)
	git->WriteJPGFromCanvas(&canvas);

	return 0;
}
