#include "scanline_artist.h"

#define PI 3.14159265358979323846
#define DEG2RAD(DEG) ((DEG)*((PI)/(180.0)))

using namespace std;
using namespace Magick;

// Scale point values to fit within width and height give
// with root at (width/2, height)
// Transformation flips y values and shifts x values
// after scaling
//void ScanlineArtist::transform(SurrogateTreeNode* tree)
//{
//	// Calculate resize scaling factors
//	double allowedWidth = 0.9*this->width;
//	double allowedHeight = 0.85*this->height;
//	double xMax = tree->findMax("x");
//	double xMin = tree->findMin("x");
//	double yMax = tree->findMax("y");
//	double yMin = tree->findMin("y");
//	printf("Mins: (%f,%f), Maxs: (%f,%f)\n",xMin,yMin,xMax,yMax);
//	double currWidth = xMax - xMin;
//	double currHeight = yMax - yMin;
//
//	double minDim = min(allowedHeight, allowedWidth);
//
//	double scalingFactorW = minDim/currWidth;
//	double scalingFactorH = minDim/currHeight;
//
//	// Transform points to look more "naturally tree-like"
//	PropertyInverter inverter(currHeight * 1.1);
//	tree->transform("y",&inverter);
//	PropertyShifter shifter(currWidth / 2);
//	tree->transform("x",&shifter);
//
//	// Scale tree values
//	tree->scale("x", scalingFactorW);
//	tree->scale("y", scalingFactorH);
//
//
//}


void ScanlineArtist::draw(Image &image, DrawableData* dataset)
{
	DiscursiveDebugPrint("Drawing\n");
	cout << endl;
	
	list<Drawable> drawList;
	drawList.push_back(DrawableStrokeWidth(0));
	//drawList.push_back(DrawableFillColor("brown"));
	
	map<int,vector<DrawableDatum*>*>::iterator rediculator = dataset->begin();
	vector<DrawableDatum*>* layerData;
	int index;
	int size;
	DrawableDatum* drawItem;
	double x;
	double y;
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
		for(vector<DrawableDatum*>::iterator dataList = layerData->begin(); dataList != layerData->end(); ++dataList)
		{
			drawItem = *dataList;
			x = drawItem->getLocationX();
			y = drawItem->getLocationY();
			//drawList.push_back(DrawableCircle(x, y, x, y + drawItem->getMass()));
			if(index != TRUNK_LAYER)
			{
				drawList.push_back(DrawableCircle(x, y, x, y + drawItem->getMass()/2));
			}
			else
			{
				drawList.push_back(DrawableCircle(x, y, x, y + 1));
			}
		}
	}
	
	image.draw(drawList);
}

