/*
 * drawable.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <vector>
#include <unordered_map>
#include "drawable_datum.h"

using namespace std;

class DrawableData
{
private:
	unordered_map<int,vector<DrawableDatum*>*> drawItems;
public:
	DrawableData();
	void insert(int layer, DrawableDatum* datum);
	vector<DrawableDatum*>* getLayer(int layer);
};

#endif /* DRAWABLE_H_ */
