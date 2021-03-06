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
//	double xMax = tree->findMax(TreeNodeKey::X);
//	double xMin = tree->findMin(TreeNodeKey::X);
//	double yMax = tree->findMax(TreeNodeKey::Y);
//	double yMin = tree->findMin(TreeNodeKey::Y);
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
//	tree->transform(TreeNodeKey::Y,&inverter);
//	PropertyShifter shifter(currWidth / 2);
//	tree->transform(TreeNodeKey::X,&shifter);
//
//	// Scale tree values
//	tree->scale(TreeNodeKey::X, scalingFactorW);
//	tree->scale(TreeNodeKey::Y, scalingFactorH);
//
//
//}


void ScanlineArtist::draw(Image &image, DrawableData* dataset)
{
	DiscursiveDebugPrint("scanline artist","Drawing\n");
	
	list<Drawable> drawList;
	drawList.push_back(DrawableStrokeWidth(0));
	//drawList.push_back(DrawableFillColor("brown"));
	
	map<int,vector<MinDrawableDatum*>*>::iterator rediculator = dataset->begin();
	vector<MinDrawableDatum*>* layerData;
	int index;
	int size;
	MinDrawableDatum* drawItem;
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
		DiscursivePrint("Drawing layer %d with %d items\n", index, layerData->size());
		for(vector<MinDrawableDatum*>::iterator dataList = layerData->begin(); dataList != layerData->end(); ++dataList)
		{
			drawItem = *dataList;
			x = drawItem->locationX;
			y = drawItem->locationY;
			//drawList.push_back(DrawableCircle(x, y, x, y + drawItem->getMass()));
			if(index != TRUNK_LAYER)
			{
				drawList.push_back(DrawableCircle(x, y, x, y + drawItem->mass/2));
			}
			else
			{
				drawList.push_back(DrawableCircle(x, y, x, y + 1));
			}
		}
	}
	
	image.draw(drawList);
}

