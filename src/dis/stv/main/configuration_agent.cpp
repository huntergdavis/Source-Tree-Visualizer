/*
 * configuration_agent.cpp
 *
 *  Created on: Oct 1st, 2010
 *      Author: Hunter Davis
 */

#include "configuration_agent.h"
#include <iomanip>
#include <cmath>
extern DiscursiveDebugAgent debugAgent;

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::ConfigurationAgent
// PURPOSE :: instantiation of ConfigurationAgent
//         :: defaults all values
// PARAMETERS :: None
// RETURN :: None
// -------------------------------------------------------------------------
ConfigurationAgent::~ConfigurationAgent()
{
};
// -------------------------------------------------------------------------
// API :: ConfigurationAgent::ConfigurationAgent
// PURPOSE :: instantiation of ConfigurationAgent
//         :: defaults all values
// PARAMETERS :: None
// RETURN :: None
// -------------------------------------------------------------------------
ConfigurationAgent::ConfigurationAgent():colorMap(new unordered_map<std::string,Magick::Color*>())
{
	// agentName equates to the passed command line parameter argument string
	agentName.clear();

	// our sranded randomizer token, called once
	/* initialize random seed: */
	srand(time(NULL));

	// config file name defaults to ./simple_tree.config
	configFileName = "./simple_tree.config";
	
	// should we read config file from stdin?
	readConfigFromStdIn = 0;

	// type of agent to use, ie command line options or interactive
	agentType = 0;

	// image size options for jpg output
	imageWidth = 500;
	imageHeight = 500;

	// scaling factors to use for spatial displacement
	// Default is unity
	startWidth = imageWidth;
	startHeight = imageHeight;

	// our filename for output file
	fileName = "tree.jpg";
	outputFileNumber = 1000;
	
	// our background image name
	backgroundImageName = "";
	
	// default color
	defaultLeafColor = "black";
	defaultTrunkColor = "brown";

	// should we make many jpgs?
	manyJpgs = 0;

	// options for many jpgs
	jpgStep = 1;
	jpgStart = 3;
	jpgStop = 10000;

	// should we output a jpg per revision?
	revJpgs = 0;

	// options for jpg per revison
	revStep = 1;
	revStart = 2;
	revStop = 10000;

	// draw filtered leaves?
	drawFilteredLeaves = 0;

	// generate filters?
	generativeFilters = 0;

};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::display_usage
// PURPOSE :: outputs usage options for simple_tree
//         ::
// PARAMETERS :: None
// RETURN :: None
// -------------------------------------------------------------------------
void ConfigurationAgent::displayUsage( void )
{
	std::string usage_string = "\nsimple_tree [option] [optionstringorint] \n";
	usage_string += "where option can be any of the following";
	usage_string += "\n-c option - expects ./configFileName.config \n";
	usage_string += "\n-p option - pass in config file on stdin\n";
	usage_string += "\n-g option - expects ~/Projects/source_tree_vis\n";
	usage_string += "\n-G option - expects gitHubusername:gitHubprojectname\n";
	usage_string += "\n-S option - expects http://hkit.googlecode.com/svn/trunk/\n";
	usage_string += "\n-C option - expects pserver:anonymous@bnetd.cvs.sourceforge.net:/cvsroot/bnetd\n";
	usage_string += "\n-i option - interactive mode (asks you questions) \n";
	usage_string += "\n-d option - debug level, expects a debug keyword\n";
	usage_string += "\n-b option - background image, expects an image filename\n";
	usage_string += "\n-n option - image numbering value, defaults to 1000\n";
	usage_string += "\n-o or -O option - output file name, defaults to tree.jpg\n";
	usage_string += "\n-m option - output the creation of the current tree one step at a time via many .images in sequence\n";
	usage_string += "\n--------------also expects the following start:stop:step i.e. 1:400:5       \n";
	usage_string += "\n----------------start number for many jpg tree rendering, default is 3\n";
	usage_string += "\n----------------finish number for many jpg tree rendering, default is treesize\n";
	usage_string += "\n----------------step value for many jpg tree rendering, default is 1\n";
	usage_string += "\n-R and -r option -output the creation of the current tree one revision at a time via many .images in sequence\n";
	usage_string += "\n-------------- -R also expects the following start:stop:step i.e. 1:400:5       \n";
	usage_string += "\n----------------start number for many jpg tree rendering, default is 3\n";
	usage_string += "\n----------------finish number for many jpg tree rendering, default is currentrevision\n";
	usage_string += "\n----------------step value for many jpg tree rendering, default is 1\n";
	usage_string += "\n-l option - spatial displacement scaling levels, expects W1:H1:W2:H2 defaults to 5000:5000:4500:4200\n";
	usage_string += "\n----------------where W1 is the start width\n";
	usage_string += "\n----------------where H1 is the start height\n";
	usage_string += "\n-z option - image size , expects WxH, defaults to 500x500 \n";


	DiscursivePrint("%s",usage_string.c_str());
    DiscursiveError("Copyright Discursive Labs LLC, 2010");
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::returnAgentName
// PURPOSE :: returns the agent name
//         ::
// PARAMETERS :: None
// RETURN :: std::string agentName - name of agent
// -------------------------------------------------------------------------
std::string ConfigurationAgent::returnAgentName()
{
	return agentName;
}

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::returnFileName
// PURPOSE :: returns the file name
//         ::
// PARAMETERS :: None
// RETURN :: std::string fileName - name of file to output
// -------------------------------------------------------------------------
std::string ConfigurationAgent::returnFileName()
{
	return fileName;
};


// -------------------------------------------------------------------------
// API :: ConfigurationAgent::returnFileName
// PURPOSE :: returns the file name
//         ::
// PARAMETERS :: None
// RETURN :: std::string fileName - name of file to output
// -------------------------------------------------------------------------
std::string ConfigurationAgent::returnBackgroundImageName()
{
	return backgroundImageName;
}

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::returnDefaultLeafColor
// PURPOSE :: returns the DefaultColor
//         ::
// PARAMETERS :: None
// RETURN :: std::string fileName - name of file to output
// -------------------------------------------------------------------------
std::string ConfigurationAgent::returnDefaultLeafColor()
{
	return defaultLeafColor;
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::returnDefaultTrunkColor
// PURPOSE :: returns the DefaultColor
//         ::
// PARAMETERS :: None
// RETURN :: std::string fileName - name of file to output
// -------------------------------------------------------------------------
std::string ConfigurationAgent::returnDefaultTrunkColor()
{
	return defaultTrunkColor;
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::parseConfigFile
// PURPOSE :: sets values based on config file input
//         ::
// PARAMETERS :: none
// RETURN :: None
// -------------------------------------------------------------------------
void ConfigurationAgent::parseConfigFile()
{

	// create our libxml structures
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    // use libxml to read and verify our config file name
    if(readConfigFromStdIn == 1)
    {
		doc = xmlReadFd(0,NULL, NULL, 0);
	}
	else
	{
		doc = xmlReadFile(configFileName.c_str(), NULL, 0);
	}

	if (doc == NULL)
	  {
			 return;
	  }
	else
	  {
			// Get the root element node
			root_element = xmlDocGetRootElement(doc);

			// create a current node for the loop
			xmlNode *cur_node = root_element;

			cur_node = cur_node->xmlChildrenNode;
			while (cur_node != NULL)
			{
				if ((!xmlStrcmp(cur_node->name, (const xmlChar *)"simple_tree_options")))
				{
					cur_node = cur_node->xmlChildrenNode;
					break;
				}
				else
				{
					cur_node = cur_node->next;
				}
			}

		// loop over all our simple tree options
		for (cur_node = cur_node; cur_node; cur_node = cur_node->next)
		{
		  if (cur_node->type == XML_ELEMENT_NODE) {
			  DiscursiveDebugPrint("xml,configuration","node type: Element, name: %s\n node type: element, value %s\n", cur_node->name, xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1));
			  char* nodeData;
			  nodeData = (char*)xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
			  if(nodeData != NULL)
			  {
				  // input filter needs to test for all metatags
				  if(strcmp((char*)cur_node->name,"input_filter") == 0)
				  {
					  SetInputFilters(doc,cur_node->xmlChildrenNode,nodeData);
				  }
				  else
				  {
					  setOptionByName((char*)cur_node->name,nodeData);
				  }
			  }
			  delete nodeData;
		  }
		}

		/*
		* free the document
		*/
		xmlFreeDoc(doc);

  }
    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::SetInputFilters
// PURPOSE :: sets input filter and metatag data
//         ::
// PARAMETERS :: xmlDoc *doc - current xml parsing document
//            :: xmlNode *cur_node - current node in xml tree for parsing
//            :: std::string filterNames - current filter names
// RETURN :: None
// -------------------------------------------------------------------------
void ConfigurationAgent::SetInputFilters(xmlDoc *doc, xmlNode *cur_node,std::string filterNames)
{
	// create a temporary storage vector for filterProperties
	std::vector<filterKeyProperty> keyProperties;

	// loop over node and look for meta tags to add
	for (cur_node = cur_node; cur_node; cur_node = cur_node->next)
	{
	  if (cur_node->type == XML_ELEMENT_NODE)
	  {
			filterKeyProperty singleFKP;
			singleFKP.keyPropertyName = (char*)cur_node->name;
			singleFKP.keyPropertyValue = (char*)xmlNodeListGetString(doc, cur_node->xmlChildrenNode, 1);
			if(singleFKP.keyPropertyName == "color")
			{
				cacheColor(singleFKP.keyPropertyValue);
			}
			keyProperties.push_back(singleFKP);
	  }
	}

	// now that we have all metatags for these values, split the values and add metatags
	// use a stringstream to split
	std::stringstream ss(filterNames);

	// store each delimited item in string
	std::string item;

	// split and push into global store
	while(std::getline(ss, item, ','))
	{
		// create a filter keystore item with all properties
		filterKeystoreItem singleFKI;
		singleFKI.keyName = item;
		singleFKI.keyProperties = keyProperties;

		// push keystore item with all properties onto main filter keystore
		filterKeyStore.push_back(singleFKI);
	}


};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::parseCommandLine
// PURPOSE :: sets values based on command line input
//         ::
// PARAMETERS :: int argc, char ** argv - the cli arguments to the main program
// RETURN :: None
// -------------------------------------------------------------------------
void ConfigurationAgent::parseCommandLine(int argc, char **argv)
{
	// our option string
	static const char *optString = "g:G:S:C:O:o:m:pb:R:l:z:n:c:ridh?";

	// if a new config file is passed, parse it
	bool newConfig = 0;

	// loop over our command options in the normal unix way

	int opt;
	opt = getopt( argc, argv, optString );
	while( opt != -1 ) {
		switch( opt ) {
			case 'c':
				configFileName = optarg;
				newConfig = 1;
				break;
			case 'g':
				agentName = optarg;
				agentType = 1;
				break;
			case 'G':
				agentName = optarg;
				agentType = 2;
				break;
			case 'S':
				agentName = optarg;
				agentType = 3;
				break;
			case 'C':
				agentName = optarg;
				agentType = 4;
				break;
			case 'b':
				backgroundImageName = optarg;
				break;
			case 'p':
				readConfigFromStdIn = 1;
				newConfig = 1;
				break;
			case 'l':
				sscanf(optarg,"%d:%d",&startWidth,&startHeight);
				break;
			case 'z':
				sscanf(optarg,"%dx%d",&imageWidth,&imageHeight);
				break;
			case 'm':
				manyJpgs = 1;
				sscanf(optarg,"%d:%d:%d",&jpgStart,&jpgStop,&jpgStep);
				if(jpgStart < 3)
				{
					jpgStart = 3;
				}
				break;
			case 'n':
				sscanf(optarg,"%d",&outputFileNumber);
				break;
			case 'R':
				revJpgs = 1;
				sscanf(optarg,"%d:%d:%d",&revStart,&revStop,&revStep);
				if(revStart < 3)
				{
					revStart = 3;
				}
				break;
			case 'r':
				revJpgs = 1;
				revStart = 3;
				revStop = 100000;
				revStep = 1;
				break;
			case 'o':
			case 'O':
				fileName = optarg;
				break;
			case 'd':
				// set the debug keyword
				debugAgent.AddDebugKeywords(optarg);
				break;
			case 'i':
				// set the interactivity level
				agentType = 5;
				break;
			case 'h':   /* fall-through is intentional */
			case '?':
				displayUsage();
				break;
			default:
				DiscursiveError("bad cli, bad!");
				break;
		}
		// get the next Command Line option
		opt = getopt( argc, argv, optString );
	}

	// re-parse any passed-in config files
	if(newConfig == 1)
	{
		parseConfigFile();
	}
}

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::setOptionByName
// PURPOSE :: sets an option value by string name
//         ::
// PARAMETERS ::  std::string optionName - name of option top set
//                std::string optionvalue - the value to set
// RETURN :: none
// -------------------------------------------------------------------------
void ConfigurationAgent::setOptionByName(std::string optionName, std::string optionValue)
{
	if((optionValue == "") || (optionValue.empty()))
	{
		DiscursiveError("Blank Value for Tag %s",optionName.c_str());
	}
    if(optionName == "file_name")
	{
		fileName = optionValue;
	}
	else if(optionName == "background_image")
	{
		backgroundImageName = optionValue;
	}
	else 	if(optionName == "image_width")
	{
		imageWidth  = atoi(optionValue.c_str());
	}
	else 	if(optionName == "image_height")
	{
		imageHeight = atoi(optionValue.c_str());
	}
	else 	if(optionName == "start_width")
	{
		startWidth = atoi(optionValue.c_str());
	}
	else 	if(optionName == "start_height")
	{
		 startHeight = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "output_file_number")
	{
		 outputFileNumber = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "many_jpgs")
	{
		 manyJpgs = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "jpg_step")
	{
		 jpgStep = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "jpg_start")
	{
		 jpgStart = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "jpg_stop")
	{
		 jpgStop = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "rev_jpgs")
	{
		 revJpgs = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "rev_step")
	{
		 revStep = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "rev_start")
	{
		 revStart = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "rev_stop")
	{
		 revStop = atoi(optionValue.c_str());;
	}
	else 	if(optionName == "debug")
	{
		debugAgent.AddDebugKeywords(optionValue);
	}
	else 	if(optionName == "input_ignore_filter")
	{
		ParseInverseKeywords(optionValue);
	}
	else if(optionName == "draw_filtered_leaves")
	{
		drawFilteredLeaves = atoi(optionValue.c_str());
	}
	else if(optionName == "generative_filters")
	{
		generativeFilters = atoi(optionValue.c_str());
	}
	else if(optionName == "default_leaf_color")
	{
		defaultLeafColor = optionValue;
		cacheColor(optionValue);
	}
	else if(optionName == "default_trunk_color")
	{
		defaultTrunkColor = optionValue;
		cacheColor(optionValue);
	}	
	else if(optionName == "repo_type")
	{
		if(optionValue == "git")
		{
			agentType = 1;
		}
		else if(optionValue == "github")
		{
			agentType = 2;
		}
		else if(optionValue == "svn")
		{
			agentType = 3;
		}
		else if(optionValue == "cvs")
		{
			agentType = 4;
		}
		else if(optionValue == "interactive")
		{
			agentType = 5;
		}
		else
		{
			DiscursiveError("Wrong repository type: %s\n",optionValue.c_str());
		}
	}
	else if(optionName == "repo_string")
	{
		agentName = optionValue.c_str();
	}
    else
	{
		// rufus: what's your favorite number?
	    DiscursiveError("Failure to set key value: \n" + optionName);
	}
};


// -------------------------------------------------------------------------
// API :: ConfigurationAgent::ParseInverseKeywords
// PURPOSE :: adds inverse keywords to the inverse keyword vector
//         ::
// PARAMETERS :: std::string inverseKeyWords - comma separated keywords
// RETURN :: void
// -------------------------------------------------------------------------
void ConfigurationAgent::ParseInverseKeywords(std::string inverseKeyWords)
{
	// split string by commas and push into global store
	// use a stringstream to split
	std::stringstream ss(inverseKeyWords);

	// store each delimited item in string
	std::string item;

	// lexical search variable
	size_t found;

	// split and push into global store
	while(std::getline(ss, item, ','))
	{
		// reset found variable
		found = std::string::npos;

	    // check for negative keyword matches
		for(std::vector<std::string>::iterator i = inverseFilterKeyStore.begin(); i != inverseFilterKeyStore.end(); ++i)
		{
			if(inverseKeyWords.find(i->c_str()) != std::string::npos)
			{
				found = 1;
			}
		}

		// if negative keywords not found, add
		if(found == 1)
		{
			inverseFilterKeyStore.push_back(item);
		}
	}
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::returnOptionByName
// PURPOSE :: returns the integer option value by std::string name
//         ::
// PARAMETERS :: std::string optionvalue - the name of value to retrieve
// RETURN :: int value - the value retrieved or an error code
// -------------------------------------------------------------------------
int ConfigurationAgent::returnOptionByName(std::string optionName)
{
	if(optionName == "agentType")
	{
		return agentType;
	}
	else 	if(optionName == "imageWidth")
	{
		return imageWidth;
	}
	else 	if(optionName == "imageHeight")
	{
		return imageHeight;
	}
	else 	if(optionName == "startWidth")
	{
		return startWidth;
	}
	else 	if(optionName == "startHeight")
	{
		return startHeight;
	}
	else 	if(optionName == "outputFileNumber")
	{
		return outputFileNumber;
	}
	else 	if(optionName == "manyJpgs")
	{
		return manyJpgs;
	}
	else 	if(optionName == "jpgStep")
	{
		return jpgStep;
	}
	else 	if(optionName == "jpgStart")
	{
		return jpgStart;
	}
	else 	if(optionName == "jpgStop")
	{
		return jpgStop;
	}
	else 	if(optionName == "revJpgs")
	{
		return revJpgs;
	}
	else 	if(optionName == "revStep")
	{
		return revStep;
	}
	else 	if(optionName == "revStart")
	{
		return revStart;
	}
	else 	if(optionName == "revStop")
	{
		return revStop;
	}
	else 	if(optionName == "drawFilteredLeaves")
	{
		return drawFilteredLeaves;
	}
	else
	{
		// rufus: what's your favorite number?
	    DiscursiveError("Failure to lookup key value" + optionName);
		return -69;
	}

};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::initializeRepositoryType
// PURPOSE :: initializes the repository type based on initial values
//         ::
// PARAMETERS :: none
// RETURN :: RepositoryAccess* - repository access type
// -------------------------------------------------------------------------
RepositoryAccess* ConfigurationAgent::initializeRepositoryType()
{
	// let an interactive or command line agent create the repository access type
	switch(agentType)
	{
		case 5:
			return interactive_agent();
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			DiscursiveDebugPrint("configuration,agents","%s\n",agentName.c_str());
			return noninteractive_agent(agentType, agentName);
			break;
		case 0:
			displayUsage();
			exit(0);
			break;
        default:
            /* You won't actually get here. */
        	DiscursiveError("an impossibility occured in error processing");
        	exit(1);
            break;
	}
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::DoesThisStringContainFilterKeywords
// PURPOSE :: tests if any internal filters match
//         :: returns the keyword number if so
//         ::
// PARAMETERS :: std::String textualData - string to search
//            :: 
// RETURN :: int - first keyword reference number if there's a match
//        :: int - returns zero if there are no filterKeyStore items
// -------------------------------------------------------------------------
int ConfigurationAgent::DoesThisStringContainFilterKeywords(std::string textualData)
{
    // store the string iterations
    int keyWordIterator = 0;
    size_t found = 0;
    // store textualData size
    size_t searchKeySize = textualData.size();
	size_t keyNameSize;

    // check for negative keyword matches and fail if found
	for(std::vector<std::string>::iterator i = inverseFilterKeyStore.begin(); i != inverseFilterKeyStore.end(); ++i)
	{
		found = textualData.find(i->c_str());
		if(found != std::string::npos)
		{
			keyNameSize = i->size();
			if((searchKeySize - found) == keyNameSize)
			{			
				return -1;
			}
		}
	}

    // if no negatives and there is no keystore size, return
    if(filterKeyStore.size() == 0)
    {
    	return 0;
    }

    // check for positive keyword matches
	for(std::vector<filterKeystoreItem>::iterator i = filterKeyStore.begin(); i != filterKeyStore.end(); ++i)
	{
		keyWordIterator++;
		found = textualData.find(i->keyName);
		if(found != std::string::npos)
		{
			keyNameSize = i->keyName.size();
			if((searchKeySize - found) == keyNameSize)
			{			
				return keyWordIterator;
			}			
		}
	}

	// if nothing is found in either bin, and we have both bins, we fail
	return -1;
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::AddFilterPropertiesToTreeNode
// PURPOSE :: adds node properties for each filter match
//         :: to the current node
//         ::
// PARAMETERS :: SurrogateTreeNode* treeNode - node to add properties to
//            :: std::string searchKey - searchKey to match filter strings to
//            :: int nodeType - o is leaf, 1 is trunk
// RETURN :: void
// -------------------------------------------------------------------------
void ConfigurationAgent::AddFilterPropertiesToTreeNode(SurrogateTreeNode* treeNode,std::string searchKey)
{
	// lexical search results
	size_t found = 0;
	size_t colorFound  = 0;

	bool isFile = treeNode->isFile;

	//for each filter property which matches, add the property to the node
    // loop over all filter types
	for(std::vector<filterKeystoreItem>::iterator i = filterKeyStore.begin(); i != filterKeyStore.end(); ++i)
	{
		int searchForItem = 0;
		if(isFile)
		{
			found = searchKey.find(i->keyName);
			if(found != std::string::npos)
			{
				searchForItem = 1;
				size_t keyNameSize = i->keyName.size();
				size_t searchKeySize = searchKey.size();
				if((searchKeySize - found) != keyNameSize)
				{
					searchForItem = 0;
				}
			}
		}

		if(searchForItem != 0)
		{
		    // add all properties to surrogatetreenode
			for(std::vector<filterKeyProperty>::iterator j = i->keyProperties.begin(); j != i->keyProperties.end(); ++j)
			{
				if(j->keyPropertyName == "color")
				{
					colorFound = 1;
				}
				treeNode->set(j->keyPropertyName,j->keyPropertyValue);
			}
		}
	}
	
	// add default color to node if not previously added
	if(colorFound == 0)
	{
		if(isFile)
		{
			if(generativeFilters == 1)
			{
				treeNode->set("color",returnGenerativeColor(searchKey));
			}
			else if(generativeFilters == 0)
			{
				treeNode->set("color",returnDefaultLeafColor());
			}
		}
		else
		{
			treeNode->set("color",returnDefaultTrunkColor());
		}
	}
};


// -------------------------------------------------------------------------
// API :: ConfigurationAgent::returnGenerativeColor
// PURPOSE :: returns a color for the type of file passed in
//         :: files with no extension get their own filter
//         ::
// PARAMETERS :: std::string searchKey - the filename
// RETURN :: std::string (color) - the stringified color value
// -------------------------------------------------------------------------
std::string ConfigurationAgent::returnGenerativeColor(std::string searchKey)
{
	// test if this searchKey matches any known keywords
	int filterFound = DoesThisStringContainFilterKeywords(searchKey);

	// generative color to be returned
	std::string generatedColor = returnDefaultLeafColor();

	// if we have no matches, we're going to do the following
	// 1.  derive a new filter
	// 2.  derive a new color
	// 3.  add the new filter with derived color
	if(filterFound <= 0)
	{
		// derive a new filter
		// a filter is a filter key, and a key property
		filterKeystoreItem singleFKI;

		// set the key property with a random color
		filterKeyProperty singleFKP;
		singleFKP.keyPropertyName = "color";

		// derive a new color and cache it
		generatedColor = deriveNewColor();
		cacheColor(generatedColor);
		singleFKP.keyPropertyValue = generatedColor;

		// search for a period
		size_t periodSearch = searchKey.find(".");

		// if there is no period, the whole search key is the filter
		// else the filter is the extension
		if(periodSearch != std::string::npos)
		{
			singleFKI.keyName = searchKey.substr(periodSearch,(searchKey.size()-periodSearch));
		}
		else
		{
			singleFKI.keyName = searchKey;
		}

		// add the color property to the properties list
		singleFKI.keyProperties.push_back(singleFKP);

		// add the new filter with derived color to master list
		filterKeyStore.push_back(singleFKI);
	}

	// return the color generated
	DiscursiveDebugPrint("color","Returning Generated Color:%s\n",generatedColor.c_str());
	return generatedColor;
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::deriveNewColor
// PURPOSE :: derives a new color
//         ::
//         ::
// PARAMETERS :: none
// RETURN :: stringified color
// -------------------------------------------------------------------------
std::string ConfigurationAgent::deriveNewColor()
{
	// storage space for hexified color string
	std::stringstream localColorString;

	// storage space for the furthest mathematically from other colors
	std::stringstream furthestLocalColorString;

	//our strings are in hex mode
	localColorString << std::hex << std::setfill('0');
	furthestLocalColorString << std::hex << std::setfill('0');

	// colorspace search timeout variable
	int searchTimeout = 100;

	// storage for distance metric
	double distance = 0;
	double bestDistance = 0;

	while(searchTimeout > 0)
	{
		// add 6 random numbers to string
		for(int i = 0;i<6;i++)
		{
			localColorString << std::setw(1) << rand() %16;
		}

		distance = minDistanceFromColorMap(localColorString.str());

		if(distance > bestDistance)
		{
			bestDistance = distance;
			furthestLocalColorString.str("");
			furthestLocalColorString.clear();
			furthestLocalColorString << std::hex << std::setfill('0');
			furthestLocalColorString << localColorString.str();
		}

		//decrement search timeout and clear color
		searchTimeout--;
		localColorString.str("");
		localColorString.clear();
		localColorString << std::hex << std::setfill('0');
	}
	// return the string
	return "#" + furthestLocalColorString.str();
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::minDistanceFromColorMap
// PURPOSE :: return minimum distance of a color from colormap
//         ::
//         ::
// PARAMETERS :: std::string color
// RETURN :: double minimum distance
// -------------------------------------------------------------------------
double ConfigurationAgent::minDistanceFromColorMap(std::string color)
{
	// init distance and minDistance storage
	double distance = 0;
	double minDistance = 9999;

	// loop over color map and find minimum distance from colors
	for(unordered_map<std::string,Magick::Color*>::iterator itr = colorMap->begin();itr!=colorMap->end();++itr)
	{
		distance = colorDistance(itr->first,color);
		if(distance < minDistance)
		{
			minDistance = distance;
		}
	}

	return minDistance;
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::colorDistance
// ATTRIB :: basic algorithm based on math from http://www.compuphase.com/cmetric.htm
// PURPOSE :: return distance between 2 hex colors
//         ::
//         ::
// PARAMETERS :: std::string color1, std::string color2
// RETURN :: double distance between colors
// -------------------------------------------------------------------------
double ConfigurationAgent::colorDistance(std::string color1, std::string color2)
{
	// storage for our RGB values
	long r1,r2,g1,g2,b1,b2;

	// convert color1
	r1 = hexStringToLong(color1.substr(1,2));
	g1 = hexStringToLong(color1.substr(3,2));
    b1 = hexStringToLong(color1.substr(5,2));

	// convert color2
	r2 = hexStringToLong(color2.substr(1,2));
	g2 = hexStringToLong(color2.substr(3,2));
    b2 = hexStringToLong(color2.substr(5,2));

	// return the sqrt mean distance between two colors
	long rmean = ( r1 + r2 ) / 2;
	long r = r1 - r2;
	long g = g1 - g2;
	long b = b1 - b2;
	return sqrt((((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8));
};


// -------------------------------------------------------------------------
// API :: ConfigurationAgent::hexStringToLongm
// PURPOSE :: converts hex string to long
//         ::
//         ::
// PARAMETERS :: std::string hexString - string to convert
// RETURN :: double distance between colors
// -------------------------------------------------------------------------
long ConfigurationAgent::hexStringToLong(std::string hexString)
{
	// stringstream to convert string hex to long value
	std::stringstream hexConverter;

	// storage for return value
	long retValue;

    // convert  hex string to int with SS
	hexConverter << std::hex << hexString;
	hexConverter >> retValue;
	return retValue;
};



// -------------------------------------------------------------------------
// API :: ConfigurationAgent::PrintFilterProperties
// PURPOSE :: prints all filter properties
//         ::
//         ::
// PARAMETERS ::
// RETURN ::
// -------------------------------------------------------------------------
void ConfigurationAgent::PrintFilterProperties()
{
	DiscursivePrint("\nFilter Properties\n");

    // print all positive filter keywords
	for(std::vector<filterKeystoreItem>::iterator i = filterKeyStore.begin(); i != filterKeyStore.end(); ++i)
	{
		DiscursivePrint("Filter Item: %s\n",i->keyName.c_str());

	    // print all positive filter keywords
		for(std::vector<filterKeyProperty>::iterator j = i->keyProperties.begin(); j != i->keyProperties.end(); ++j)
		{
			DiscursivePrint("--%s: %s\n",j->keyPropertyName.c_str(),j->keyPropertyValue.c_str());
		}
	}
}

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::ReturnColorMap
// PURPOSE :: returns the color map
//         ::
//         ::
// PARAMETERS ::
// RETURN :: unordered_map<std::string,Magick::ColorRGB> color map structure
// -------------------------------------------------------------------------
unordered_map<std::string, Magick::Color*>* ConfigurationAgent::getColorMap()
{
	return colorMap;
};

// -------------------------------------------------------------------------
// API :: ConfigurationAgent::CacheColor
// PURPOSE :: cache the string color as a Magic color
//         ::
//         ::
// PARAMETERS :: std::string - string color value
// RETURN :: int - status value
// -------------------------------------------------------------------------
int ConfigurationAgent::cacheColor(std::string colorString)
{
	// store our local color
	size_t redColor;
	size_t blueColor;
	size_t greenColor;
	
	// first we validate the string
	if(colorString == "blue")
	{
		redColor = 0;
		blueColor = 255;
		greenColor = 0;
	}
	else if(colorString == "black")
	{
		redColor = 0;
		blueColor = 0;
		greenColor = 0;		
	}
	else if(colorString == "red")
	{
		redColor = 255;
		blueColor = 0;
		greenColor = 0;		
	}
	else if(colorString == "green")
	{
		redColor = 0;
		blueColor = 0;
		greenColor = 255;		
	}
	else if(colorString == "yellow")
	{
		redColor = 255;
		blueColor = 255;
		greenColor = 0;		
	}
	else if(colorString == "brown")
	{
		redColor = 204;
		blueColor = 153;
		greenColor = 0;		
	}
	else if(colorString == "white")
	{
		redColor = 255;
		blueColor = 255;
		greenColor = 255;		
	}
	else if(colorString == "pink")
	{
		redColor = 255;
		blueColor = 155;
		greenColor = 103;		
	}
	else if(colorString.find('#') == 0)
	{
		// convert the hex string to decimal
		 redColor = strtoul(colorString.substr(1,2).c_str(),NULL, 16);
		 greenColor = strtoul(colorString.substr(3,2).c_str(),NULL, 16);
		 blueColor = strtoul(colorString.substr(5,2).c_str(),NULL, 16);
	}
	else
	{
		// return 0
		return 0;
	}

	DiscursiveDebugPrint("color","\ncolor: r%d,g%d,b%d \n",redColor,greenColor,blueColor);
	// now we add the color value in as decimal
	Magick::Color* colorToAdd = new Magick::ColorRGB(redColor/255.0,greenColor/255.0,blueColor/255.0);
	(*colorMap)[colorString] = colorToAdd;
	return 1;
	
};

