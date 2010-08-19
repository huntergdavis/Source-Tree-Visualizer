/*
 * simple_tree.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */
#include <Magick++.h>
#include "../model/surrogate_tree_node.h"
#include "../io/git_repository_access.h"
#include "../dec/spatial_displacement.h"
#include "../draw/scanline_artist.h"
#include "../gen/space_colonizer.h"

using namespace Magick;

int main(int argc, char **argv)
{
	// Tree image size parameters
	int WIDTH = 500;
	int HEIGHT = 500;

	// Tree Generation pipeline
	// Retrieve tree from GIT and create surrogate tree
	GitRepositoryAccess* git = new GitRepositoryAccess("~/Projects/source_tree_vis");
	SurrogateTreeNode* source = git->retrieve();
	printf("Source tree name is %s\n", source->data["name"].c_str());

	// Decorate surrogate tree nodes with locations
	SpatialDisplacement* disp = new SpatialDisplacement(WIDTH,HEIGHT);
	disp->decorate(source);

	// Digitize decorated surrogate tree into line segment tree
	SpaceColonizer* digitizer = new SpaceColonizer(3);
	DrawableData* lines = digitizer->digitize(source);

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
		cout << "Caught exception: " << error_.what() << endl;
		return 1;
	}

	return 0;
}
