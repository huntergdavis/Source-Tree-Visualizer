/*
 * drawable.h
 *
 *  Created on: Aug 12, 2010
 *      Author: Mark Christensen
 */

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include <vector>
#include <map>
#include "drawable_datum.h"
#include "../system/discursive_system.h"


using namespace std;

class DrawableData
{
private:
	map<int,vector<MinDrawableDatum*>*> *drawItems;
public:
	DrawableData();
	~DrawableData();
	void insert(int layer, MinDrawableDatum* datum);
	vector<MinDrawableDatum*>* getLayer(int layer);
	map<int,vector<MinDrawableDatum*>*>::iterator begin();
	map<int,vector<MinDrawableDatum*>*>::iterator end();
};

#endif /* DRAWABLE_H_ */
