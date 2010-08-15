/*
 * simple_tree.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */
#include <Magick++.h>
#include <boost/property_tree/ptree.hpp>
#include "../model/surrogate_tree_node.h"
#include "../io/git_repository_access.h"
#include "../dec/spatial_displacement.h"
#include "../draw/scanline_artist.h"
#include "../gen/space_colonizer.h"

using namespace Magick;

int main(int argc, char **argv)
{
	// Tree Generation pipeline
	// Retrieve tree from GIT and create surrogate tree
	GitRepositoryAccess* git = new GitRepositoryAccess("~/Projects/source_tree_vis");
	//boost::property_tree::ptree* source = git->retrieve();
	SurrogateTreeNode* source = git->retrieve();

	// Decorate surrogate tree nodes with locations
	SpatialDisplacement* disp = new SpatialDisplacement();
	disp->decorate(source);

	// Digitize decorated surrogate tree into line segment tree
	SpaceColonizer* digitizer = new SpaceColonizer(3);
	DrawableData* lines = digitizer->digitize(source);

	// Draw tree
	InitializeMagick(*argv);
	int WIDTH = 500;
	int HEIGHT = 500;
	Image canvas(Geometry(WIDTH,HEIGHT),"white");
	ScanlineArtist* artist = new ScanlineArtist();
	artist->draw(canvas, lines);

	return 0;
}
