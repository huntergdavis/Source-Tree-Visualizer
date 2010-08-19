/*
 * scanline_artist.h
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#ifndef SCANLINE_ARTIST_H_
#define SCANLINE_ARTIST_H_

#include <iostream>
#include <math.h>
#include <list>

#include "artist.h"
#include "../model/surrogate_tree_node.h"
#include "../model/property_inverter.h"
#include "../model/property_shifter.h"

using namespace std;
using namespace Magick;

class ScanlineArtist : public Artist
{
public:
	static const int WIDTH = 450;
	static const int HEIGHT = 400;
	static const int STEP = 1;

private:
	void transform(SurrogateTreeNode* tree);

public:
	void draw(Image &image, DrawableData* dataset);
};

#endif /* SCANLINE_ARTIST_H_ */
