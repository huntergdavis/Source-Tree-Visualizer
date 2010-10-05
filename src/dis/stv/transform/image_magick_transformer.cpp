/*
 * image_magick_transformer.cpp
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#include "./image_magick_transformer.h"

//ImageMagickTransformer::ImageMagickTransformer(int height, int width):height(height),width(width)
//{
//}
//
//ImageMagickTransformer::~ImageMagickTransformer()
//{
//}

void ImageMagickTransformer::transform(va_list args)
{

	SurrogateTreeNode* tree = va_arg(args,SurrogateTreeNode*);
	int width = va_arg(args,int);
	int height = va_arg(args,int);
	// Calculate resize scaling factors
	double allowedWidth = 0.95*width;
	double allowedHeight = 0.95*height;
	double xMax = tree->findMax(TreeNodeKey::X);
	double xMin = tree->findMin(TreeNodeKey::X);
	double yMax = tree->findMax(TreeNodeKey::Y);
	double yMin = tree->findMin(TreeNodeKey::Y);
	DiscursiveDebugPrint("Mins: (%f,%f), Maxs: (%f,%f)\n",xMin,yMin,xMax,yMax);
	double currWidth = xMax - xMin;
	double currHeight = yMax - yMin;

	double minDim = min(allowedHeight, allowedWidth);

	double scalingFactorW = minDim/currWidth;
	double scalingFactorH = minDim/currHeight;

	// Transform points to look more "naturally tree-like"
	tree->scale(TreeNodeKey::Y, scalingFactorH);
	PropertyInverter inverter(height * 0.98);
	tree->transform(TreeNodeKey::Y,&inverter);

	PropertyShifter shifter(-1*((xMax + xMin) / 2));
	tree->transform(TreeNodeKey::X,&shifter);
	// Scale tree values
	tree->scale(TreeNodeKey::X, scalingFactorW);
	PropertyShifter shifter2(minDim / 2.0);
	tree->transform(TreeNodeKey::X,&shifter2);
}
