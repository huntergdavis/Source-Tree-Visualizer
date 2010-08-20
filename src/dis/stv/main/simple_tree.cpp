/*
 * simple_tree.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */
#include <Magick++.h>
#include "../model/surrogate_tree_node.h"
#include "../dec/spatial_displacement.h"
#include "../draw/scanline_artist.h"
#include "../gen/space_colonizer.h"
#include "../system/discursive_system.h"
#include "./interactive_agent.h"

using namespace Magick;

int main(int argc, char **argv)
{
	// set the debug level
	SetDiscursiveDebugLevel(1);
	// Tree image size parameters
	int WIDTH = 500;
	int HEIGHT = 500;

	// Tree Generation pipeline
	// create an abstract method for repository access
	RepositoryAccess* git;

	// let an interactive agent create the repository access type
	git = interactive_agent();


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
