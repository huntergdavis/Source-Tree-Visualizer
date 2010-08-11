// xml.cpp - header file for functions related to git tree parsing and pulling

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "xml.h"


// -------------------------------------------------------------------------
// API :: XmlLeafProcessor::XmlLeafProcessor
// PURPOSE :: instantiation of xml leaf processor
//         :: 
// PARAMETERS ::
// RETURN :: None
// -------------------------------------------------------------------------
XmlLeafProcessor::XmlLeafProcessor()
{
	XmlStorage = "<xml> ";
}

// -------------------------------------------------------------------------
// API :: XmlLeafProcessor::~XmlLeafProcessor
// PURPOSE :: destructor of xml leaf processor
//         :: 
// PARAMETERS ::
// RETURN :: None
// -------------------------------------------------------------------------
XmlLeafProcessor::~XmlLeafProcessor()
{
}

// -------------------------------------------------------------------------
// API :: XmlLeafProcessor::AddLeafToXml
// PURPOSE :: add leaf xml to xml processor
//         :: 
// PARAMETERS :: int x : x value for leaf
//            :: int y : y value for leaf
//            :: int rotation : rotation value for leaf
// RETURN :: int return status
// -------------------------------------------------------------------------
int XmlLeafProcessor::AddLeafToXml(int x, int y, int rotation)
{

	// stringstream for integer processing
	std::stringstream intConvertStringStreamX;
	std::stringstream intConvertStringStreamY;
	std::stringstream intConvertStringStreamRot;
	std::string tempStreamStorage;

	// add leaf node
	XmlStorage += "<leaf>";

	// add x value
	XmlStorage += "<x>";
	intConvertStringStreamX << x;
	intConvertStringStreamX >> tempStreamStorage;
	XmlStorage += tempStreamStorage;
	tempStreamStorage = "";
	XmlStorage += "</x>";

	// add y value
	XmlStorage += "<y>";
        intConvertStringStreamY << y;
        intConvertStringStreamY >> tempStreamStorage;
        XmlStorage += tempStreamStorage;
	XmlStorage += "</y>";
	
	// add rotation value
	XmlStorage += "<rotation>";
        intConvertStringStreamRot << rotation;
        intConvertStringStreamRot >> tempStreamStorage;
        XmlStorage += tempStreamStorage;
	XmlStorage += "</rotation>";
	
	// close leaf tag
	XmlStorage += "</leaf>";

	// return a status
	return 1;
}

// -------------------------------------------------------------------------
// API :: XmlLeafProcessor::GenerateXmlFile
// PURPOSE :: instantiation of xml leaf processor
//         :: 
// PARAMETERS :: std::string fileName - filename to be written to 
// RETURN :: int - return status
// -------------------------------------------------------------------------
int XmlLeafProcessor::GenerateXmlFile(std::string fileName)
{
	// open a file handle to fileName
	std::fstream xmlFile(fileName.c_str(), std::ios::out);
	
	// write the xml data set minus closing tags
	xmlFile << XmlStorage.c_str();
	xmlFile << "\n</xml>\n";
	
	// close the file handle
	xmlFile.close();

	// return a status
	return 1;
}

