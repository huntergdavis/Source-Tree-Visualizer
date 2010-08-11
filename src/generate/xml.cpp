// xml.cpp - header file for functions related to git tree parsing and pulling

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
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
	// add leaf node
	XmlStorage += "<leaf>";

	// add x value
	XmlStorage += "<x>";
	XmlStorage += x;
	XmlStorage += "</x>";

	// add y value
	XmlStorage += "<y>";
	XmlStorage += y;
	XmlStorage += "</y>";
	
	// add rotation value
	XmlStorage += "<rotation>";
	XmlStorage += rotation;
	XmlStorage += "</rotation>";
	
	// close leaf tag
	XmlStorage += "</leaf>";
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
}

