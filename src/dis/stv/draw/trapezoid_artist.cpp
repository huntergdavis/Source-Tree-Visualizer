/*
 * trapezoid_artist.cpp
 *
 *  Created on: Oct 06, 2010
 *      Author: Mark Christensen
 */

#include "./trapezoid_artist.h"

#define PI 3.14159265358979323846
#define DEG2RAD(DEG) ((DEG)*((PI)/(180.0)))

using namespace std;
using namespace Magick;


void TrapezoidArtist::draw(Image &image, DrawableData* dataset)
{
	DiscursiveDebugPrint("Drawing\n");
	
	list<Drawable> drawList;
	drawList.push_back(DrawableStrokeWidth(0));
	
	map<int,vector<MinDrawableDatum*>*>::iterator rediculator = dataset->begin();
	vector<MinDrawableDatum*>* layerData;
	int index;
	int size;
	MinDrawableDatum* drawItem;
	double x;
	double y;
	double endX;
	double endY;
	double step;
	double dx;
	double dy;
	for(; rediculator != dataset->end(); ++rediculator)
	{
		index = rediculator->first;
		switch(index)
		{
			case TRUNK_LAYER:
				drawList.push_back(DrawableFillColor("brown"));
				size = 1;
				break;
			case LEAF_LAYER:
				drawList.push_back(DrawableFillColor("green"));
				size = 5;
				break;
			case DEBUG_LAYER:
				drawList.push_back(DrawableFillColor("blue"));
				size = 2;
				break;
		}
		layerData = rediculator->second;
		DiscursivePrint("Drawing layer %d with %d items\n", index, layerData->size());
		for(vector<MinDrawableDatum*>::iterator dataList = layerData->begin(); dataList != layerData->end(); ++dataList)
		{
			drawItem = *dataList;
			x = drawItem->locationX;
			y = drawItem->locationY;
			if(index != TRUNK_LAYER)
			{
				drawList.push_back(DrawableCircle(x, y, x, y + 1));
			}
			else
			{
				list<Coordinate> trapezoid;
				dx = (drawItem->mass/2.0) * cos(drawItem->angle);
				dy = -1 * (drawItem->mass/2.0) * sin(drawItem->angle);
				// Left base pt
				trapezoid.push_back(Coordinate(x - dy,y + dx));
				// Right base pt
				trapezoid.push_back(Coordinate(x + dy,y - dx));
				endX = x + drawItem->extent * cos(drawItem->angle);
				endY = y - drawItem->extent * sin(drawItem->angle);
				dx = (drawItem->mass * drawItem->massRatio/2.0) * cos(drawItem->angle);
				dy = -1 * (drawItem->mass * drawItem->massRatio/2.0) * sin(drawItem->angle);
				trapezoid.push_back(Coordinate(endX + dy,endY - dx));
				trapezoid.push_back(Coordinate(endX - dy,endY + dx));
				printf("Drawing polygon [");
				Coordinate p;
				for(list<Coordinate>::iterator pt = trapezoid.begin(); pt != trapezoid.end(); ++pt)
				{
					p = *pt;
					printf("<%d,%d>,",(int)p.x(),(int)p.y());
				}
				printf("]\n");
				drawList.push_back(DrawableCircle(x, y, x, y + (drawItem->mass/2.0)));
				drawList.push_back(DrawablePolygon(trapezoid));
			}
		}
	}
	
	image.draw(drawList);
}

