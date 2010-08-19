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
	// for now, ask the user if local files or github project
	cout << "\nPress 1 and enter for local, 2 and enter for github\n";
	int answer = 0 * argc;
	cin >> answer;

	GitRepositoryAccess* git;
	if(answer == 1)
	{
		git = new GitRepositoryAccess("~/Projects/source_tree_vis");
	}
	else if(answer == 2)
	{
		git = new GitRepositoryAccess("mojombo","grit");
	}

	SurrogateTreeNode* source = git->retrieve();
	printf("Source tree name is %s\n", source->data["name"].c_str());

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
