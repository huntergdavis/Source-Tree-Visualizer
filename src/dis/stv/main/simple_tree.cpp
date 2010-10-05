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
#include "../dec/decorator_factory.h"
#include "../gen/digitizer_factory.h"
#include "../transform/transform_factory.h"
#include "../model/surrogate_tree_node.h"
#include "../dec/spatial_displacement.h"
#include "../draw/scanline_artist.h"
#include "../gen/space_colonizer.h"
#include "../transform/transformer.h"


using namespace Magick;
int debugLevel;

int main(int argc, char **argv)
{
	// initialize libmagick
	InitializeMagick(*argv);

	// set the debug level
	SetDiscursiveDebugLevel(0);

	// initialize our timing class
	DiscursiveTime timerAgent("Entire SourceTreeVis Program");

	// initialize our configuration agent
	ConfigurationAgent configAgent;

	// parse our config file and also pre-check command line
	configAgent.parseConfigFile();

	// parse our command line
	configAgent.parseCommandLine(argc,argv);

	// Tree Generation pipeline
	// create an abstract method for repository access
	RepositoryAccess* git = configAgent.initializeRepositoryType();

    // set our repository single or manyjpg options
	git->snapshotJpgs = configAgent.returnOptionByName("manyJpgs");
	git->jpgIndex = configAgent.returnOptionByName("outputFileNumber");

	git->fileName = configAgent.returnFileName();
	git->startWidth = (double)configAgent.returnOptionByName("startWidth");
	git->startHeight = (double)configAgent.returnOptionByName("startHeight");
	git->imageWidth = configAgent.returnOptionByName("imageWidth");
	git->imageHeight = configAgent.returnOptionByName("imageHeight");
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
	DiscursivePrint("Retrieving From Server, Please Wait...\n");
	timerAgent.Tic("Initial Retrieve");
	git->source = git->retrieve();
	DiscursivePrint("Server Retrieval Took:\n");
	timerAgent.PrintToc("Initial Retrieve");

	// output tree info
	DiscursivePrint("Source tree name is %s\n", git->source->data[TreeNodeKey::NAME].c_str());
	DiscursivePrint("Source tree has %d revisions which require %d tree inserts\n",git->globalRevs,git->globalInserts);

	delete(git->source);
	git->source = NULL;

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
	DiscursiveDebugPrint("Total revs in repo = %d and loopstop = %d\n",git->globalRevs,loopStop);
	if((executeLoopType == 3) && (loopStop >= git->globalRevs))
	{
		loopStop = git->globalRevs-1;
	}

	// loop over a bunch of increasingly large trees for debug or revision tree generation
	// use our user-set parameters to define our step
	for(int i = loopStart; i<loopStop;i+= loopStep)
	{

		// reset our insert variables
		if(executeLoopType == 2)
		{
			git->localInserts = 0;
			git->insertTarget = i;
		}
		else if(executeLoopType == 3)
		{
			git->localRevs = 0;
			git->revTarget = i;
		}

		// retrieve our source tree
		git->source = git->retrieve();


		// Decorate surrogate tree nodes with locations
		DiscursivePrint("Decorating surrogate trees %d out of %d step value %d\n",i,loopStop,loopStep);
		timerAgent.Tic("Decorating surrogate trees");
//		SpatialDisplacement* disp = new SpatialDisplacement(git->imageWidth,git->imageHeight,scaleWidth,scaleHeight);
		int decoratorType = DecoratorFactory::SPATIAL_DISPLACEMENT_LEAF_CLUSTERING;  //SPATIAL_DISPLACEMENT_NAIVE;
//		Decorator* decorator = DecoratorFactory::getInstance(decoratorType, 4, git->imageWidth,git->imageHeight,widthRescaling,heightRescaling);
		Decorator* decorator = DecoratorFactory::getInstance(decoratorType, 0);
		decorator->decorate(git->source);
		timerAgent.PrintToc("Decorating surrogate trees");

		// Transform coordinates
		DiscursivePrint("Transforming coordinates to create %d x %d image\n",git->startWidth, git->startHeight);
		timerAgent.Tic("Transforming tree");
		int transformerType = TransformFactory::IMAGE_MAGICK_TRANSFORMER;
//		Transformer<SurrogateTreeNode>* transformer = TransformerFactory<SurrogateTreeNode>::getInstance(transformerType,2,git->imageWidth, (int)(0.95*git->imageHeight));
		TransformFactory::transform(transformerType,3,git->source,git->startWidth, (int)(0.95*git->startHeight));
//		transformer->transform(git->source);
		timerAgent.PrintToc("Transforming tree");

		// Digitize decorated surrogate tree into line segment tree
		DiscursivePrint("Digitizing decorated surrogate trees into line segment trees %d out of %d step value %d\n",i,loopStop,loopStep);
		timerAgent.Tic("Digitizing decorated trees into line segments");
		int segmentLength = 1;
//	    SpaceColonizer *digitizer = new SpaceColonizer(segmentLength);
		int digitizerType = DigitizerFactory::SPACE_COLONIZER;
		Digitizer* digitizer = DigitizerFactory::getInstance(digitizerType,1,segmentLength);
		DrawableData* lines = digitizer->digitize(git->source);
		timerAgent.PrintToc("Digitizing decorated trees into line segments");

		// Draw tree
		DiscursivePrint("Drawing Tree %d out of %d step value %d\n",i,loopStop,loopStep);
		timerAgent.Tic("Drawing Tree with artist.draw");
		Image canvas(Geometry(git->startWidth,git->startHeight),"white");
		ScanlineArtist artist;
		artist.draw(canvas, lines);
		timerAgent.PrintToc("Drawing Tree with artist.draw");

		// Transform image
		DiscursivePrint("Transforming image of size %d x %d to create %d x %d image\n",git->startWidth,git->startHeight,git->imageWidth, git->imageHeight);
		timerAgent.Tic("Transforming image");
		transformerType = TransformFactory::IMAGE_RESIZE_TRANSFORMER;
//		Transformer<Image>* imageTransformer = TransformerFactory<Image>::getInstance(transformerType,2,git->imageWidth, (int)(0.95*git->imageHeight));
		TransformFactory::transform(transformerType,3,&canvas,git->imageWidth,git->imageHeight);
//		imageTransformer->transform(&canvas);
		timerAgent.PrintToc("Transforming image");

		// actually generate a tree
		timerAgent.Tic("actually generating image from canvas");
		git->WriteJPGFromCanvas(&canvas);
		timerAgent.PrintToc("actually generating image from canvas");

		delete digitizer;
		delete decorator;

//		if(git->source != NULL)
//		{
//			delete(git->source);
//			git->source = NULL;
//		}
		DiscursivePrint("\n");
	}

	// finish the timing on our entire program
	timerAgent.Toc("Entire SourceTreeVis Program");
	timerAgent.PrintRunningTotals();
	return 0;
}

//template TransformFactory<ImageMagickTransformer>;
//template TransformFactory<ImageResizeTransformer>;
