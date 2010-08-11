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
	XmlStorage = "<xml> "
}

XmlLeafProcessor::~XmlLeafProcessor()
{
}

int XmlLeafProcessor::AddLeafToXml(int x, int y, int rotation)
{
	// add leaf node
	XmlStorage += "<leaf>";

	// add x value
	XmlStorage += "<x>"
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

int XmlLeafProcessor::GenerateXmlFile(std::string fileName)
{
}

