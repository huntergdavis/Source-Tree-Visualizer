/*
 * repository_access.cpp
 *
 *  Created on: Sept 22, 2010
 *      Author: HunterD
 */


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


int RepositoryAccess::GenerateTreeAndWriteJPG()
{

	// Tree image size parameters
	int WIDTH = 500;
	int HEIGHT = 500;
	SurrogateTreeNode* source = retrieve();
	std::string sourceTreeNameOutput = "Source tree name is";
	sourceTreeNameOutput += source->data["name"].c_str();
	DiscursiveDebugPrint(sourceTreeNameOutput);

	// Decorate surrogate tree nodes with locations
	SpatialDisplacement* disp = new SpatialDisplacement(WIDTH,HEIGHT,scaleWidth,scaleHeight);
	disp->decorate(source);

	// Digitize decorated surrogate tree into line segment tree
	SpaceColonizer* digitizer = new SpaceColonizer(2);
	DrawableData* lines = digitizer->digitize(source);

	// Transform

	// Draw tree
	InitializeMagick("/");
	Image canvas(Geometry(WIDTH,HEIGHT),"white");
	ScanlineArtist* artist = new ScanlineArtist();
	artist->draw(canvas, lines);

	// Generate Image Name with preceding integer and increment it
	std::stringstream integerPlusFileName;
	integerPlusFileName << jpgIndex;
	integerPlusFileName << fileName;
	jpgIndex++;

	try
	{
		// Write the image to a file with a preceding integer
		canvas.write(integerPlusFileName.str().c_str());
	}
	catch( Exception &error_ )
	{
		DiscursiveError(error_.what());
	}

	return 0;
};
