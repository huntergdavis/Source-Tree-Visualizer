/*
 * drawable_data.cpp
 *
 *  Created on: Aug 18, 2010
 *      Author: programmer
 */

#include "drawable_data.h"

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

