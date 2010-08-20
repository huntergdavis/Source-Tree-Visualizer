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

void DrawableData::insert(int layer, DrawableDatum* datum)
{
	vector<DrawableDatum*>* layerList = this->getLayer(layer);
	if(layerList == NULL)
	{
		layerList = new vector<DrawableDatum*>();
		this->drawItems[layer] = layerList;
	}

	layerList->push_back(datum);
}


vector<DrawableDatum*>* DrawableData::getLayer(int layer)
{
	vector<DrawableDatum*>* layerList = this->drawItems[layer];
	return layerList;
}

map<int,vector<DrawableDatum*>*>::iterator DrawableData::begin()
{
	return this->drawItems.begin();
}

map<int,vector<DrawableDatum*>*>::iterator DrawableData::end()
{
	return this->drawItems.end();
}