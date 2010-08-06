#include <Magick++.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace Magick;

int WIDTH = 450;
int HEIGHT = 400;
int STEP = 4;

typedef struct branch_str
{
	// Count of all child leaves
	int totalChildren;
	int subBranches;
	branch_str **branches;
} Branch;

// Angle is measured from standard cartesian y=0, x>0 (right)
void drawBranch(Image &image, Branch *branch, int rootSize, int length, int x, int y, int angle)
{
	printf("Drawing branch: 0x%x\n",(unsigned int)branch);
	cout << endl;

	// Calculate x-diff, y-diff & r-diff values
	int finalX = x + (length * cos((angle/180.0)*3.14159));
	int finalY = y + (length * sin((angle/180.0)*3.14159));
	int finalR = (int)(0.75*rootSize);
	int steps = (length / STEP) + 1;
	cout << "...will take " << steps << " steps." << endl;
	
	double xDiff = (finalX - x)/steps;
	double yDiff = (finalY - y)/steps;
	double rDiff = (finalR - rootSize)/steps;
	
	
	// Draw branch
	double radius = rootSize/2;
	double _x = x;
	double _y = y;
	for(int i = 0; i < steps; i++)
	{
		cout << " - step " << i << endl;
		// Draw circle
		cout << "Drawing circle at (" << _x << "," << _y << ")" << endl;
		image.draw(DrawableCircle((int)radius, (int)radius, _x, _y));
		_x += xDiff;
		_y += yDiff;
		radius += rDiff;
	}
	// Create other branches
	int _angle = angle;
	for(int i = 0; i < branch->subBranches; i++)
	{
		if(branch->subBranches > 1)
		{
			_angle = angle + 20 - ((110/(branch->subBranches-1))-10+(rand()%15))*i;
		}
		drawBranch(image, branch->branches[i], rootSize*(((double)branch->branches[i]->totalChildren)/branch->totalChildren), length*((rand()%10)/10.0), _x, _y, _angle);
	}
}

void drawTrunk(Image &image, Branch trunk, int rootSize, int height)
{
	printf("Drawing trunk: 0x%x\n",(unsigned int)(&trunk));
	cout << endl;
	image.fillColor("brown");
	int radius = rootSize/2;
	// Center horizontally
	int x = (WIDTH/2) - radius;
	// Start trunk at bottom of image and grow up
	int y = HEIGHT - radius;
	for(; y > HEIGHT - height - radius; y -= STEP)
	{
		// Draw circle
		cout << "Drawing circle at (" << x << "," << y << ")" << endl;
		image.draw(DrawableCircle(radius, radius, x, y));
	}
	// Draw all branches
	int angle = 90;
	for(int i = 0; i < trunk.subBranches; i++)
	{
		if(trunk.subBranches > 1)
		{
			angle = 170 - ((170/(trunk.subBranches-1))-10+(rand()%15))*i;
		}
		drawBranch(image, trunk.branches[i], rootSize*(((double)trunk.branches[i]->totalChildren)/trunk.totalChildren), height*((rand()%10)/10.0), x, y, angle);
	}
}

int main(int argc,char **argv)
{
    InitializeMagick(*argv);
    
    // Setup branches
    Branch trunk;
    trunk.totalChildren = 100;
    trunk.subBranches = 2;
    trunk.branches = (Branch**)malloc(trunk.subBranches*sizeof(Branch*));
    
    Branch* sub1 = new Branch;
    sub1->totalChildren = 65;
    sub1->subBranches = 4;
    trunk.branches[0] = sub1;
    sub1->branches = (Branch**)malloc(sub1->subBranches*sizeof(Branch*));
    Branch* sub11 = new Branch;
    sub1->totalChildren = 10;
    sub1->subBranches = 0;
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
    sub12->branches[0] = sub131;
    Branch* sub14 = new Branch;
    sub14->totalChildren = 5;
    sub14->subBranches = 0;
    sub1->branches[3] = sub14;
    
    Branch* sub2 = new Branch;
    sub2->totalChildren = 35;
    sub2->subBranches = 2;
    trunk.branches[1] = sub2;
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

    // Construct the image object. Seperating image construction from the
    // the read operation ensures that a failure to read the image file
    // doesn't render the image object useless.
    Image tree(Geometry(WIDTH,HEIGHT),"white");
    
    // Start with drawing trunk.  This method iteratively draws the 
    // branches as well.
    drawTrunk(tree, trunk, 45, 75);

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
