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

using namespace std;
using namespace Magick;

class ScanlineArtist : public Artist
{
public:
	static const int WIDTH = 450;
	static const int HEIGHT = 400;
	static const int STEP = 1;

private:

	typedef struct branch_str
	{
		// Count of all child leaves
		int totalChildren;
		int subBranches;
		branch_str **branches;
	} Branch;

	void drawBranch(list<Drawable> *drawList, Branch *branch, int startSize, int endSize, int length, int x, int y, int angle);

public:
	void draw(Image &image, const DrawableData* dataset);
};

#endif /* SCANLINE_ARTIST_H_ */
