/*
 * image_magick_transformer.h
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#ifndef IMAGE_MAGICK_TRANSFORMER_H_
#define IMAGE_MAGICK_TRANSFORMER_H_

#include "./transformer.h"
#include "./property_inverter.h"
#include "./property_shifter.h"
#include "../model/surrogate_tree_node.h"

class ImageMagickTransformer: public Transformer
{
public:
//	int height;
//	int width;
//	ImageMagickTransformer(int height, int width);
//	virtual ~ImageMagickTransformer();
	static void transform(va_list args);
};

#endif /* IMAGE_MAGICK_TRANSFORMER_H_ */
