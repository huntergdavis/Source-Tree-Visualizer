/*
 * configuration_agent.h
 *
 *  Created on: Oct 1st, 2010
 *      Author: Hunter Davis
 */

#ifndef CONFIGURATIONL_AGENT_H_
#define CONFIGURATION_AGENT_H_

#include "initial_agents.h"
#include <libxml/xmlreader.h>
#include <Magick++.h>
#include "../model/surrogate_tree_node.h"
#include "../io/repository_access.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <math.h>
#include <float.h>



class filterKeyProperty
{
public:
	std::string keyPropertyName;
	std::string keyPropertyValue;
};

class filterKeystoreItem
{
public:
	std::string keyName;
	std::vector<filterKeyProperty> keyProperties;
};


class ConfigurationAgent
{
private:
	// agentName equates to the passed command line parameter argument string
	std::string agentName;

	// type of agent to use, ie command line options or interactive
	int agentType;

	// image size options for jpg output
	int imageWidth;
	int imageHeight;

	// scaling factors to use for spatial displacement
	// Default is unity
	int startWidth;
	int startHeight;
	
	// default colors
	std::string defaultLeafColor;
	std::string defaultTrunkColor;
	
	// color map for all colors in tree
	unordered_map<std::string,Magick::Color*>* colorMap;
	
	// our background image name
	std::string backgroundImageName;

	// our filename for output file
	std::string fileName;
	int outputFileNumber;

	// our html filename
	std::string htmlFileName;

	// our xml filename
	std::string xmlFileName;

	// our config file name
	std::string configFileName;
	
	// should we read a config file in from stdin?
	int readConfigFromStdIn;

	// keystores - vectors of keyword and metatag pairs
	std::vector<filterKeystoreItem> filterKeyStore;
	std::vector<std::string> inverseFilterKeyStore;

	// should we output html ?
	int htmlOutputToFile;

	// should we output xml ?
	int xmlOutputToFile;

	// should we index our output files with numbers?
	int indexOutputFiles;

	// should we generate filters ?
	int generativeFilters;

	// should we draw filtered leaves?
	int drawFilteredLeaves;

	// should we make many jpgs?
	int manyJpgs;

	// options for many jpgs
	int jpgStep;
	int jpgStart;
	int jpgStop;

	// should we output a jpg per revision?
	int revJpgs;

	// options for jpg per revison
	int revStep;
	int revStart;
	int revStop;

	// private functions for parsing filter keywords and properties
	void ParseInverseKeywords(std::string inverseKeywords);
	void SetInputFilters(xmlDoc *doc,xmlNode *cur_node, std::string filterNames);

	// private function for returning a generative color string
	std::string returnGenerativeColor(std::string searchKey);

	// private function for returning the distance between two color strings
	double colorDistance(std::string color1, std::string color2);

	// private function to convert hex strings to longs
	long hexStringToLong(std::string hexString);


public:
	ConfigurationAgent();
	~ConfigurationAgent();
	void displayUsage();
	void parseCommandLine(int argc, char **argv);
	void parseConfigFile();
	std::string returnAgentName();
	std::string returnFileName();
	std::string returnHTMLFileName();
	std::string returnXMLFileName();
	std::string returnBackgroundImageName();
	std::string returnDefaultLeafColor();
	std::string returnDefaultTrunkColor();
	int returnOptionByName(std::string optionName);
	void setOptionByName(std::string optionName, std::string optionValue);
	int DoesThisStringContainFilterKeywords(std::string textualData);
	void AddFilterPropertiesToTreeNode(SurrogateTreeNode* treeNode,std::string searchKey);
	void PrintFilterProperties();
	std::string returnHTMLFilterProperties();
	std::string returnXMLFilterProperties();
	double minDistanceFromColorMap(std::string color);
	
	void writeXmlAndHtmlToFile();
	void incrementOutputFileNumbering(int increment);

	// color cache and lookup functions
	unordered_map<std::string,Magick::Color*>* getColorMap();
	int cacheColor(std::string colorString);
	RepositoryAccess* initializeRepositoryType();
	std::string deriveNewColor();

};


#endif /*CONFIGURATION_AGENT_H_ */
