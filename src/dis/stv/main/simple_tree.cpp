/*
 * simple_tree.cpp
 *
 *  Created on: Aug 12, 2010
 *      Author: programmer
 */

#include "../system/discursive_system.h"
#include "./configuration_agent.h"
#include "./repository_access.h"
#include <Magick++.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unordered_map>
#include "../dec/decorator_factory.h"
#include "../gen/digitizer_factory.h"
#include "../transform/transform_factory.h"
#include "../model/surrogate_tree_node.h"
#include "../dec/spatial_displacement.h"
#include "../draw/scanline_artist.h"
#include "../draw/trapezoid_artist.h"
#include "../gen/space_colonizer.h"
#include "../transform/transformer.h"
#include "../../util/system_monitor.h"


using namespace Magick;
DiscursiveDebugAgent debugAgent;
ConfigurationAgent configAgent;

int main(int argc, char **argv)
{
	// initialize our configuration agent
	// parse our config file and also pre-check command line
	configAgent.parseConfigFile();

	// parse our command line
	configAgent.parseCommandLine(argc,argv);

	System::debugProcessMemory("Config Init");

	// initialize libmagick
	InitializeMagick(*argv);
	System::debugProcessMemory("ImageMagick++ Init");

	// initialize our timing class
	DiscursiveTime timerAgent("Entire SourceTreeVis Program");

	// store our XML
	std::string megaXmlString;

	// Tree Generation pipeline
	// create an abstract method for repository access
	RepositoryAccess* git = configAgent.initializeRepositoryType();

    // set our repository single or manyjpg options
	git->snapshotJpgs = configAgent.returnOptionByName("manyJpgs");

	// set our textual repository options
	git->drawFilteredLeaves = configAgent.returnOptionByName("drawFilteredLeaves");
	git->fileName = configAgent.returnFileName();
	git->startWidth = (double)configAgent.returnOptionByName("startWidth");
	git->startHeight = (double)configAgent.returnOptionByName("startHeight");
	git->imageWidth = configAgent.returnOptionByName("imageWidth");
	git->imageHeight = configAgent.returnOptionByName("imageHeight");
	git->backgroundImageName = configAgent.returnBackgroundImageName();
	// set our scaling factor from our received or default options
	git->scaleWidth = git->imageWidth / git->startWidth;
	git->scaleHeight = git->imageHeight / git->startHeight;
	git->globalInserts = 0;
	git->localInserts = 0;
	git->insertTarget = 0;
	git->revJpgs = configAgent.returnOptionByName("revJpgs");
	git->globalRevs = 0;
	git->localRevs = 0;
	git->revTarget = 0;
	git->currentTreeSize = 0;
	git->logGenerated = 0;
	

	// retrieve our source tree
	DiscursiveDebugPrint("default","Retrieving From Server, Please Wait...\n");
	timerAgent.Tic("Initial Retrieve");
	git->source = git->retrieve();
	git->source->set(TreeNodeKey::COLOR, configAgent.returnDefaultTrunkColor());

	DiscursiveDebugPrint("time","Server Retrieval Took:\n");
	timerAgent.Toc("Initial Retrieve");

	// output tree info
	DiscursiveDebugPrint("default","Source tree name is %s\n", git->source->data[TreeNodeKey::NAME].c_str());
	DiscursiveDebugPrint("default","Source tree has %d revisions which require %d tree inserts\n",git->globalRevs,git->globalInserts);

	// read in background image if exists
	// create a background image variable for lookup in loop
	bool useBackgroundImage = 0;
	timerAgent.Tic("Background Retrieve and scale");
	Image backgroundImage;
	if(git->backgroundImageName != "")
	{
		backgroundImage.read(git->backgroundImageName);
		// scale image
		backgroundImage.scale(Geometry(git->startWidth,git->startHeight));
		useBackgroundImage = 1;
	}
	timerAgent.Toc("Background Retrieve and scale");

	delete(git->source);
	git->source = NULL;

	// revjpgs can store previous known good canvas element
	// this dramatically decreases processing speed
	Image previousCanvas;
	int previousInserts = 0;
	if(configAgent.returnOptionByName("revJpgs"))
	{
		Image setPreviousCanvas(Geometry(git->imageWidth,git->imageHeight),"white");
		setPreviousCanvas.matte(true);	
		previousCanvas = setPreviousCanvas;
	};

	// create loop step variables generically for any jpg looping type
	int loopStart = 0;
	int loopStop = 1;
	int loopStep = 1;
	int executeLoopType = 1;

	// check for our custom loop types
	if(configAgent.returnOptionByName("manyJpgs"))
	{
		loopStart = configAgent.returnOptionByName("jpgStart");
		loopStop = configAgent.returnOptionByName("jpgStop");
		loopStep = configAgent.returnOptionByName("jpgStep");
		executeLoopType = 2;
	}
	else if(configAgent.returnOptionByName("revJpgs"))
	{
		loopStart = configAgent.returnOptionByName("revStart");
		loopStop = configAgent.returnOptionByName("revStop");
		loopStep = configAgent.returnOptionByName("revStep");
		executeLoopType = 3;
	}

	// check for too many global inserts or revisions
	if((executeLoopType == 2) && (loopStop >= git->globalInserts))
	{
		loopStop = git->globalInserts-1;
	}
	DiscursiveDebugPrint("default","Total revs in repo = %d and loopstop = %d\n",git->globalRevs,loopStop);
	if((executeLoopType == 3) && (loopStop >= git->globalRevs))
	{
		loopStop = git->globalRevs-1;
	}

	// loop over a bunch of increasingly large trees for debug or revision tree generation
	// use our user-set parameters to define our step
	for(int i = loopStart; i<loopStop;i+= loopStep)
	{
		System::debugProcessMemory("Loop Start");
		// reset our insert variables
		if(executeLoopType == 2)
		{
			git->localInserts = 0;
			git->insertTarget = i;
		}
		else if(executeLoopType == 3)
		{
			git->localRevs = 0;
			git->localInserts = 0;
			git->revTarget = i;
		}

		// retrieve our source tree
		git->source = git->retrieve();
		git->source->set(TreeNodeKey::COLOR, configAgent.returnDefaultTrunkColor());
		System::debugProcessMemory("Tree Retrieved");

		// should we skip rendering?
		bool skipRendering = 0;
	
		// if revjpgs
		if(executeLoopType == 3)
		{	
			// if we're looping over revisions, we can re-use canvas
			// if inserts are the same, use old canvas
			//here we use old canvas
			if(git->localInserts == previousInserts)
			{
				skipRendering = 1;
				// actually generate a tree
				timerAgent.Tic("actually generating image from canvas");
				git->WriteJPGFromCanvas(&previousCanvas);
				timerAgent.Toc("actually generating image from canvas");

				// increment the output file numbering for any output files
				configAgent.incrementOutputFileNumbering(loopStep);				
			}
		
		// set previous inserts after
		previousInserts = git->localInserts;
		}

		// if we don't get to skip our rendering pipeline
		// execute the rendering pipeline
		if(skipRendering == 0)
		{
			// Decorate surrogate tree nodes with locations
			DiscursivePrint("Decorating surrogate trees %d out of %d step value %d\n",i,loopStop,loopStep);
			timerAgent.Tic("Decorating surrogate trees");
			int decoratorType = DecoratorFactory::SPATIAL_DISPLACEMENT_FULL_CLUSTERING;  //SPATIAL_DISPLACEMENT_NAIVE;
			Decorator* decorator = DecoratorFactory::getInstance(decoratorType, 0);
			decorator->decorate(git->source);
			timerAgent.Toc("Decorating surrogate trees");
			System::debugProcessMemory("Decorate");

			// print an xml file of our last surrogate tree
			timerAgent.Tic("Creating XML");
			megaXmlString = git->source->returnXml();
			timerAgent.Toc("Creating XML");

			// Transform coordinates
			DiscursivePrint("Transforming coordinates to create %d x %d image\n",git->startWidth, git->startHeight);
			timerAgent.Tic("Transforming tree");
			int transformerType = TransformFactory::IMAGE_MAGICK_TRANSFORMER;
	//		printf("Calling transform with (%d, %d, %p, %d, %d)\n",transformerType,3,git->source,git->startWidth, (int)(0.95*git->startHeight));
			TransformFactory::transform(transformerType,3,git->source,git->startWidth, (int)(0.96*git->startHeight));
			timerAgent.PrintToc("Transforming tree");
			System::debugProcessMemory("Transform");

			// Digitize decorated surrogate tree into line segment tree
			DiscursivePrint("Digitizing decorated surrogate trees into line segment trees %d out of %d step value %d\n",i,loopStop,loopStep);
			timerAgent.Tic("Digitizing decorated trees into line segments");
			int segmentLength = 1;
			int digitizerType = DigitizerFactory::SIMPLE_TRAPEZOIDER;
			unordered_map<string, Magick::Color*>* colorMap = configAgent.getColorMap();
			Digitizer* digitizer = DigitizerFactory::getInstance(digitizerType,0);
			digitizer->setColorMap(colorMap);
			DrawableData* lines = digitizer->digitize(git->source);
			timerAgent.Toc("Digitizing decorated trees into line segments");
			System::debugProcessMemory("Digitize");

			// Draw tree
			DiscursivePrint("Drawing Tree %d out of %d step value %d\n",i,loopStop,loopStep);
			timerAgent.Tic("Drawing Tree with artist.draw");
			
			// instantiate canvas and draw lines
			Image canvas(Geometry(git->startWidth,git->startHeight),"white");
			canvas.matte(true);
			if(useBackgroundImage == 1)
			{
				timerAgent.Tic("Background Image to Canvas Memory Copy");
				canvas = backgroundImage;
				timerAgent.Toc("Background Image to Canvas Memory Copy");
			}
			TrapezoidArtist artist;
			artist.draw(canvas, lines);
			timerAgent.Toc("Drawing Tree with artist.draw");
			System::debugProcessMemory("Draw");

			// Transform image
			DiscursivePrint("Transforming image of size %d x %d to create %d x %d image\n",git->startWidth,git->startHeight,git->imageWidth, git->imageHeight);
			timerAgent.Tic("Transforming image");
			transformerType = TransformFactory::IMAGE_RESIZE_TRANSFORMER;
			TransformFactory::transform(transformerType,3,&canvas,git->imageWidth,git->imageHeight);
			timerAgent.PrintToc("Transforming image");
			System::debugProcessMemory("Transform");


			if( !configAgent.returnWaterMarkFileName().empty() )
			{
				// Draw watermark
				try
				{
					Image watermark;
					watermark.read( configAgent.returnWaterMarkFileName() );
					const Geometry wgeom = watermark.size();
					Geometry geom = canvas.size();
					canvas.composite(watermark,geom.width() - wgeom.width(), geom.height() - wgeom.height(), OverCompositeOp);
				}
				catch(Exception &err)
				{
					// Couldn't load watermark, ignore.
					DiscursiveDebugPrint("watermark","Error creating watermark: %s\n", err.what());
				}
			}

			// actually generate a tree
			timerAgent.Tic("actually generating image from canvas");
			std::string waterMarkFileName = configAgent.returnWaterMarkFileName();
			git->WriteJPGFromCanvas(&canvas);
			timerAgent.Toc("actually generating image from canvas");
			System::debugProcessMemory("Write Image");

			// write out aux files if flags are set
			timerAgent.Tic("writing html and xml images to files");
			configAgent.writeXmlToFile();
			timerAgent.Toc("writing html and xml images to files");

			// increment the output file numbering for any output files
			configAgent.incrementOutputFileNumbering(loopStep);
			
			// save canvas for revjpgs optimization
			// if revjpgs
			if(executeLoopType == 3)
			{
				previousCanvas = canvas;
			}
			System::debugProcessMemory("Loop End");

			delete digitizer;
			delete decorator;

			DiscursivePrint("\n");
		}
	}

	// write out aux files if flags are set
	timerAgent.Tic("writing html and xml images to files");
	configAgent.writeHtmlToFile();
	timerAgent.Toc("writing html and xml images to files");

	// print the latest XML
	DiscursiveDebugPrint("xml","%s\n",megaXmlString.c_str());

	// finish the timing on our entire program
	timerAgent.Toc("Entire SourceTreeVis Program");

	// print out a legend, if necessary
	//configAgent.PrintFilterProperties();

	// print the filter legend as xml
	DiscursiveDebugPrint("xml,filter_output","%s\n",configAgent.returnXMLFilterProperties().c_str());

	// print the filter legend as html
	DiscursiveDebugPrint("html,filter_output","%s\n",configAgent.returnHTMLFilterProperties().c_str());

	// print out running totals for all times
	timerAgent.PrintRunningTotals();


	return 0;
}

//template TransformFactory<ImageMagickTransformer>;
//template TransformFactory<ImageResizeTransformer>;
