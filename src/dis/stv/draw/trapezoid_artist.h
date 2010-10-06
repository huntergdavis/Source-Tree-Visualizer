/*
 * trapezoid_artist.h
 *
 *  Created on: Oct 06, 2010
 *      Author: Mark Christensen
 */

#ifndef TRAPEZOID_ARTIST_H_
#define TRAPEZOID_ARTIST_H_

#include <iostream>
#include <math.h>
#include <list>

#include "./artist.h"
#include "../gen/digitizer.h"
#include "../model/surrogate_tree_node.h"
#include "../system/discursive_system.h"


using namespace std;
using namespace Magick;

class TrapezoidArtist : public Artist
{
private:
	void transform(SurrogateTreeNode* tree);

public:
	void draw(Image &image, DrawableData* dataset);
};

#endif /* TRAPEZOID_ARTIST_H_ */
