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
int debugLevel;


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
	usage_string += "\n-m option - output the creation of the current tree one step at a time via many .jpgs in sequence\n";
	usage_string += "\n--------------also expects the following start:stop:step i.e. 1:400:5       \n";
	usage_string += "\n----------------start number for many jpg tree rendering, default is 3\n";
	usage_string += "\n----------------finish number for many jpg tree rendering, default is treesize\n";
	usage_string += "\n----------------step value for many jpg tree rendering, default is 1\n";
	usage_string += "\n-R and -r option -output the creation of the current tree one revision at a time via many .jpgs in sequence\n";
	usage_string += "\n-------------- -R also expects the following start:stop:step i.e. 1:400:5       \n";
	usage_string += "\n----------------start number for many jpg tree rendering, default is 3\n";
	usage_string += "\n----------------finish number for many jpg tree rendering, default is currentrevision\n";
	usage_string += "\n----------------step value for many jpg tree rendering, default is 1\n";
	usage_string += "\n-W option - spatial displacement scaling width level, defaults to .9\n";
	usage_string += "\n-H option - spatial displacement scaling height level, defaults to .85\n";


	DiscursivePrint("%s",usage_string.c_str());
    DiscursiveError("Copyright Discursive Labs LLC, 2010");
}


int main(int argc, char **argv)
{
	InitializeMagick(*argv);
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

	// should we output a jpg per revision?
	int revJpgs = 0;

	// options for jpg per revison
	int revStep = 1;
	int revStart = 2;
	int revStop = 10000;

	// our option string
	static const char *optString = "g:G:S:C:H:W:O:m:R:ridh?";

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
				sscanf(optarg,"%d:%d:%d",&jpgStart,&jpgStop,&jpgStep);
				if(jpgStart < 3)
				{
					jpgStart = 3;
				}
				break;
			case 'R':
				revJpgs = 1;
				sscanf(optarg,"%d:%d:%d",&revStart,&revStop,&revStep);
				if(revStart < 3)
				{
					revStart = 3;
				}
				break;
			case 'r':
				revJpgs = 1;
				revStart = 3;
				revStop = 100000;
				revStep = 1;
				break;
			case 'H':
				scaleHeight = atof(optarg);
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
			printf("%s\n",agentName);
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
	git->revJpgs = revJpgs;
	git->globalRevs = 0;
	git->localRevs = 0;
	git->revTarget = 0;
	git->currentTreeSize = 0;
	git->logGenerated = 0;

	// retrieve our source tree
	git->source = git->retrieve();

	// output tree info
	DiscursivePrint("Source tree name is %s\n", git->source->data[TreeNodeKey::NAME].c_str());
	DiscursivePrint("Source tree has %d revisions which require %d tree inserts\n",git->globalRevs,git->globalInserts);

	delete(git->source);
	git->source = NULL;

	// create loop step variables generically for any jpg looping type
	int loopStart = 0;
	int loopStop = 1;
	int loopStep = 1;
	int executeLoopType = 1;

	// check for our custom loop types
	if(manyJpgs)
	{
		loopStart = jpgStart;
		loopStop = jpgStop;
		loopStep = jpgStep;
		executeLoopType = 2;
	}
	else if(revJpgs)
	{
		loopStart = revStart;
		loopStop = revStop;
		loopStep = revStep;
		executeLoopType = 3;
	}

	// check for too many global inserts or revisions
	if((executeLoopType == 2) && (loopStop >= git->globalInserts))
	{
		loopStop = git->globalInserts-1;
	}
	DiscursivePrint("Total revs in repo = %d and loopstop = %d\n",git->globalRevs,loopStop);
	if((executeLoopType == 3) && (loopStop >= git->globalRevs))
	{
		loopStop = git->globalRevs-1;
	}

	// loop over a bunch of increasingly large trees for debug or revision tree generation
	// use our user-set parameters to define our step
	for(int i = loopStart; i<loopStop;i+= loopStep)
	{

		// reset our insert variables
		if(executeLoopType == 2)
		{
			git->localInserts = 0;
			git->insertTarget = i;
		}
		else if(executeLoopType == 3)
		{
			git->localRevs = 0;
			git->revTarget = i;
		}

		// retrieve our source tree
		git->source = git->retrieve();


		DiscursivePrint("Decorating surrogate trees %d out of %d step value %d\n",i,loopStop,loopStep);
		// Decorate surrogate tree nodes with locations
		int START_WIDTH = 5000;
		int START_HEIGHT = 5000;
		int END_WIDTH = 500;
		int END_HEIGHT = 500;
		double widthRescaling = END_WIDTH/(double)START_WIDTH;
		double heightRescaling = END_HEIGHT/(double)START_HEIGHT;
		//SpatialDisplacement *disp(START_WIDTH,START_HEIGHT,scaleWidth*widthRescaling,scaleHeight*heightRescaling) = new;
		SpatialDisplacement* disp = new SpatialDisplacement(END_WIDTH,END_HEIGHT,scaleWidth,scaleHeight);
		disp->decorate(git->source);

		// Digitize decorated surrogate tree into line segment tree
		DiscursivePrint("Digitizing decorated surrogate trees into line segment trees %d out of %d step value %d\n",i,loopStop,loopStep);
	    SpaceColonizer *digitizer = new SpaceColonizer(1);
		DrawableData* lines = digitizer->digitize(git->source);

		// Transform

		// Draw tree
		DiscursivePrint("Drawing Tree %d out of %d step value %d\n",i,loopStop,loopStep);
		Image canvas(Geometry(END_WIDTH,END_HEIGHT),"white");
		ScanlineArtist artist;
		artist.draw(canvas, lines);

		// actually generate a tree
		DiscursivePrint("Writing Tree %d out of %d step value %d\n\n",i,loopStop,loopStep);
//		git->WriteJPGFromCanvas(&canvas);

		delete digitizer;
		delete disp;

//		if(git->source != NULL)
//		{
//			delete(git->source);
//			git->source = NULL;
//		}
	}
	return 0;
}
