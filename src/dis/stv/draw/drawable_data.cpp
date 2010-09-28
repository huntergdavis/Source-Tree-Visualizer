/*
 * drawable_data.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#include "drawable_data.h"
#include "../system/discursive_system.h"


DrawableData::DrawableData()
{

}

void DrawableData::insert(int layer, MinDrawableDatum* datum)
{
	vector<MinDrawableDatum*>* layerList = this->getLayer(layer);
	if(layerList == NULL)
	{
		layerList = new vector<MinDrawableDatum*>();
		this->drawItems[layer] = layerList;
	}

	layerList->push_back(datum);
}


vector<MinDrawableDatum*>* DrawableData::getLayer(int layer)
{
	vector<MinDrawableDatum*>* layerList = this->drawItems[layer];
	return layerList;
}

map<int,vector<MinDrawableDatum*>*>::iterator DrawableData::begin()
{
	return this->drawItems.begin();
}

map<int,vector<MinDrawableDatum*>*>::iterator DrawableData::end()
{
	return this->drawItems.end();
}
