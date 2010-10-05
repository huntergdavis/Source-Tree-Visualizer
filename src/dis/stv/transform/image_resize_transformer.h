/*
 * image_resize_transformer.h
 *
 *  Created on: Oct 4, 2010
 *      Author: Mark Christensen
 */

#ifndef IMAGE_RESIZE_TRANSFORMER_H_
#define IMAGE_RESIZE_TRANSFORMER_H_

#include <Magick++.h>
#include "./transformer.h"

using namespace Magick;

class ImageResizeTransformer : public Transformer
{
public:
//	ImageResizeTransformer();
//	virtual ~ImageResizeTransformer();
	static void transform(va_list args);
};

#endif /* IMAGE_RESIZE_TRANSFORMER_H_ */
