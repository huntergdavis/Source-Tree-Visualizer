#include "scanline_artist.h"

#define PI 3.14159265358979323846
#define DEG2RAD(DEG) ((DEG)*((PI)/(180.0)))

using namespace std;
using namespace Magick;

// Angle is measured from standard cartesian y=0, x>0 (right)
void ScanlineArtist::drawBranch(list<Drawable> *drawList, Branch *branch, int startSize, int endSize, int length, int x, int y, int angle)
{
	cout << "[" << branch->subBranches << "," << branch->totalChildren << "]" << endl;
	if(startSize < 2)
	{
		startSize = 2;
	}
	
	if(endSize < 2)
	{
		endSize = 2;
	}
	
	// Calculate x-diff, y-diff & r-diff values
	int xStep = (length * cos(DEG2RAD(angle)));
	int yStep = (length * sin(DEG2RAD(angle)));
	int steps = (length / STEP);
	
	double xDiff = xStep/(double)steps;
	double yDiff = yStep/(double)steps;
	double rDiff = (endSize - startSize)/(2.0*steps);
	
	//printf("Drawing branch [size[start: %u, end: %u], length: %u, step[x: %d, y: %d]]\n",startSize, endSize,length,xStep,yStep);
	//cout << "...will take " << steps << " steps." << endl;
	
	// Draw branch
	drawList->push_back(DrawableFillColor("brown"));
	double radius = startSize/2.0;
	double _x = x;
	double _y = y;
	for(int i = 0; i < steps; i++)
	{
		//cout << " - step " << i << endl;
		// Draw circle
		//cout << "Drawing circle(" << (int)radius << ") at (" << _x << "," << _y << ")" << endl;
		//image.draw(DrawableCircle(_x, _y, (int)radius, (int)radius));
		drawList->push_back(DrawableCircle(_x, _y, _x, _y + (int)radius));
		_x += xDiff;
		_y += yDiff;
		radius += rDiff;
	}
	if(branch->subBranches > 0)
	{
		// Create other branches
		double aDiff = ((160/(branch->subBranches))-5+(rand()%15));
		int _angle = -190 + (rand() % 30) + aDiff*0.5;
		for(int i = 0; i < branch->subBranches; i++)
		{
			//printf("Drawing branch at angle %d\n", angle);
			double branchFraction = ((double)branch->branches[i]->totalChildren)/branch->totalChildren;
			if(branchFraction < (1/(2.0*branch->subBranches)))
			{
				branchFraction = (1/(2.0*branch->subBranches));
			}
			//drawBranch(image, branch->branches[i], rootSize*(((double)branch->branches[i]->totalChildren)/branch->totalChildren), length*((rand()%10)/10.0), _x, _y, _angle);
			drawBranch(drawList, branch->branches[i], endSize, endSize*branchFraction, length*(((rand()%400)/1000.0) + 0.55), _x, _y, _angle);
			_angle += aDiff;
		}
	}
	else if(branch->totalChildren > 0)
	{
		// Draw leaves
		drawList->push_back(DrawableFillColor("green"));
		// Draw in circle around end of branch
		int leafRadius = 11;
		double leafBuffer = 2.5;
		int leafSize = 2*(leafRadius + leafBuffer);
		if(branch->totalChildren > 1)
		{
			int radiusIncrement = leafSize + 2*leafBuffer;
			int levels = ((int)log((branch->totalChildren-1)*leafSize/radiusIncrement) + 1);
			if(levels < 1)
			{
				levels = 1;
			}
			int radius = 0;
			int totalChildCount = 0;
			int childrenAtLevel = 0;
			for(int v = 0; v < levels; v++)
			{
				radius += radiusIncrement;
				childrenAtLevel = (int)(2.0*PI*radius/leafSize);
				totalChildCount += childrenAtLevel;
				if(totalChildCount > branch->totalChildren)
				{
					childrenAtLevel = (branch->totalChildren - totalChildCount - childrenAtLevel);
				}
				double angDiv = (2*PI)/childrenAtLevel;
				double ang = 0;
				for(int i = 0; i < childrenAtLevel; i++)
				{
					int __x = _x + (radius * (((rand()%350)/1000.0) + 0.75)) * cos(ang);
					int __y = _y + (radius * (((rand()%350)/1000.0) + 0.5)) * sin(ang);
					drawList->push_back(DrawableCircle(__x, __y, __x, __y + leafRadius));
					ang += angDiv;
				}
			}
		}
		// add leaf entry to xml generator
		//xmlForGit.AddLeafToXml(_x,_y,leafRadius);
		drawList->push_back(DrawableCircle(_x, _y, _x, _y + leafRadius));
		// Draw final node at center
		//if(branch->totalChildren > 0)
		//{
		//	drawList->push_back(DrawableCircle(_x, _y, _x, _y + leafRadius));
		//}
	}
}


void ScanlineArtist::draw(Image &image, DrawableData* dataset)
{
	printf("Drawing\n");
	cout << endl;
	
	list<Drawable> drawList;
	drawList.push_back(DrawableStrokeWidth(0));
	drawList.push_back(DrawableFillColor("brown"));
	
	map<int,vector<DrawableDatum*>*>::iterator rediculator = dataset->begin();
	vector<DrawableDatum*>* layerData;
	DrawableDatum* drawItem;
	double x;
	double y;
	for(; rediculator != dataset->end(); ++rediculator)
	{
		layerData = rediculator->second;
		for(vector<DrawableDatum*>::iterator dataList = layerData->begin(); dataList != layerData->end(); ++dataList)
		{
			drawItem = *dataList;
			x = drawItem->getLocationX();
			y = drawItem->getLocationY();
			drawList.push_back(DrawableCircle(x, y, x + 2, y + 2));
		}
	}
	
	image.draw(drawList);
}

