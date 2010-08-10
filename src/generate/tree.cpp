#include <Magick++.h>
#include <iostream>
#include <math.h>
#include <list>
#include "git.h"

#define PI 3.14159265358979323846
#define DEG2RAD(DEG) ((DEG)*((PI)/(180.0)))

using namespace std;
using namespace Magick;



int WIDTH = 450;
int HEIGHT = 400;
int STEP = 1;

typedef struct branch_str
{
	// Count of all child leaves
	int totalChildren;
	int subBranches;
	branch_str **branches;
} Branch;

// Angle is measured from standard cartesian y=0, x>0 (right)
void drawBranch(list<Drawable> *drawList, Branch *branch, int startSize, int endSize, int length, int x, int y, int angle)
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
		drawList->push_back(DrawableCircle(_x, _y, _x, _y + leafRadius));
		// Draw final node at center
		//if(branch->totalChildren > 0)
		//{
		//	drawList->push_back(DrawableCircle(_x, _y, _x, _y + leafRadius));
		//}
	}
}

void drawTree(Image &image, Branch *trunk, int rootSize, int height)
{
	printf("Drawing trunk\n");
	cout << endl;
	//image.fillColor("brown");
	
	list<Drawable> drawList;
	drawList.push_back(DrawableStrokeWidth(0));
	drawList.push_back(DrawableFillColor("brown"));
	
	int radius = rootSize/2;
	// Center horizontally
	int x = (WIDTH/2) - radius;
	// Start trunk at bottom of image and grow up
	int y = HEIGHT - radius;
	for(; y > HEIGHT - height - radius; y -= STEP)
	{
		// Draw circle
		//cout << "Drawing circle(" << (int)radius << ") at (" << x << "," << y << ")" << endl;
		//image.draw(DrawableCircle(x, y, radius, radius));
		drawList.push_back(DrawableCircle(x, y, x, y+radius));
	}
	// Draw all branches
	//int angle = 90;
	double aDiff = ((150/(trunk->subBranches))-10+(rand()%35));
	int angle = -170 - (rand() % 35) + aDiff*((rand()%800)/1000.0);
	for(int i = 0; i < trunk->subBranches; i++)
	{
		printf("Drawing branch at angle %d\n", angle);
		double branchFraction = ((double)trunk->branches[i]->totalChildren)/trunk->totalChildren;
		double weight = 1.0;
		double branchWeight = 1.0;
		if(branchFraction < 0.2)
		{
			branchFraction = 0.2;
			weight = 0.85;
			branchWeight = 0.35;
		}
		drawBranch(&drawList, trunk->branches[i], rootSize*branchWeight, rootSize*branchFraction, height*(((rand()%250)/1000.0)+0.5), x, y, angle*weight);
		angle += (aDiff * (((rand()%500)/1000.0) + 0.75));
	}
	
	image.draw(drawList);
	image.display();
}

int main(int argc,char **argv)
{

    std::string gitDirectory = "../../";
    std::string outFile = "./outputfile.ass.outfile";
    int retStatus = getCustomGitLog(gitDirectory,outFile);


    InitializeMagick(*argv);
    srand(time(0));
    
    // Setup branches
    Branch *trunk = new Branch;
    trunk->totalChildren = 100;
    trunk->subBranches = 3;
    trunk->branches = (Branch**)malloc(trunk->subBranches*sizeof(Branch*));
    
    Branch* sub1 = new Branch;
    sub1->totalChildren = 65;
    sub1->subBranches = 4;
    trunk->branches[0] = sub1;
    sub1->branches = (Branch**)malloc(sub1->subBranches*sizeof(Branch*));
    Branch* sub11 = new Branch;
    sub11->totalChildren = 10;
    sub11->subBranches = 0;
    sub1->branches[0] = sub11;
    Branch* sub12 = new Branch;
    sub12->totalChildren = 35;
    sub12->subBranches = 2;
    sub1->branches[1] = sub12;
    sub12->branches = (Branch**)malloc(sub12->subBranches*sizeof(Branch*));
    Branch* sub121 = new Branch;
    sub121->totalChildren = 11;
    sub121->subBranches = 0;
    sub12->branches[0] = sub121;
    Branch* sub122 = new Branch;
    sub122->totalChildren = 24;
    sub122->subBranches = 0;
    sub12->branches[1] = sub122;
    Branch* sub13 = new Branch;
    sub13->totalChildren = 15;
    sub13->subBranches = 1;
    sub1->branches[2] = sub13;
    sub13->branches = (Branch**)malloc(sub13->subBranches*sizeof(Branch*));
    Branch* sub131 = new Branch;
    sub131->totalChildren = 15;
    sub131->subBranches = 0;
    sub13->branches[0] = sub131;
    Branch* sub14 = new Branch;
    sub14->totalChildren = 5;
    sub14->subBranches = 0;
    sub1->branches[3] = sub14;
    
    Branch* sub2 = new Branch;
    sub2->totalChildren = 35;
    sub2->subBranches = 2;
    trunk->branches[1] = sub2;
    sub2->branches = (Branch**)malloc(sub2->subBranches*sizeof(Branch*));
    Branch* sub21 = new Branch;
    sub21->totalChildren = 15;
    sub21->subBranches = 0;
    sub2->branches[0] = sub21;
    Branch* sub22 = new Branch;
    sub22->totalChildren = 20;
    sub22->subBranches = 2;
    sub2->branches[1] = sub22;
    sub22->branches = (Branch**)malloc(sub22->subBranches*sizeof(Branch*));
    Branch* sub221 = new Branch;
    sub221->totalChildren = 12;
    sub221->subBranches = 0;
    sub22->branches[0] = sub221;
    Branch* sub222 = new Branch;
    sub222->totalChildren = 8;
    sub222->subBranches = 0;
    sub22->branches[1] = sub222;
    Branch* sub3 = new Branch;
    sub3->totalChildren = 40;
    sub3->subBranches = 0;
    trunk->branches[2] = sub3;

    // Construct the image object. Seperating image construction from the
    // the read operation ensures that a failure to read the image file
    // doesn't render the image object useless.
    Image tree(Geometry(WIDTH,HEIGHT),"white");
    
    // Start with drawing trunk->  This method iteratively draws the 
    // branches as well.
    drawTree(tree, trunk, 30, 125);

    try
    {
        // Write the image to a file
        tree.write( "out/tree.jpg" );
    }
    catch( Exception &error_ )
    {
        cout << "Caught exception: " << error_.what() << endl;
        return 1;
    }
    return 0;
}
