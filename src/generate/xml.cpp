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
	// add leaf node
	XmlStorage += "<leaf>";

	// add x value
	AddIntToXml("x",x);

	// add y value
	AddIntToXml("y",y);
		
	// add rotation value
	AddIntToXml("rotation",rotation);
	
	// close leaf tag
	XmlStorage += "</leaf>";

	// return a status
	return 1;
}



// -------------------------------------------------------------------------
// API :: XmlLeafProcessor::AddIntToXml
// PURPOSE :: add integer to xml processor
//         :: 
// PARAMETERS :: std::string tagName - name of xml tag to add
//            :: int tagValue- integer value of xml tag to add
//            :: 
// RETURN :: int return status
// -------------------------------------------------------------------------
int XmlLeafProcessor::AddIntToXml(std::string tagName,int tagValue)
{
	// string stream processor and storage mechanism
	std::stringstream intConvertStringStream;

	// convert integer value to string data
	intConvertStringStream << tagValue;
	
	return AddTagToXml(tagName,intConvertStringStream.str());
}


// -------------------------------------------------------------------------
// API :: XmlLeafProcessor::AddTagToXml
// PURPOSE :: add generic xml to xml processor
//         :: 
// PARAMETERS :: std::string tagName - name of xml tag to add
//            :: std::string tagValue- value of xml tag to add
//            :: 
// RETURN :: int return status
// -------------------------------------------------------------------------
int XmlLeafProcessor::AddTagToXml(std::string tagName,std::string tagValue)
{

	std::string tempStreamStorage;
	
	// add opening tag to temporary storage
	tempStreamStorage = "<";
	tempStreamStorage += tagName;
	tempStreamStorage += ">";
	
	// add value to temporary storage
	tempStreamStorage += tagValue;

	// add closing tag to temporary storage
	tempStreamStorage += "</";
	tempStreamStorage += tagName;
	tempStreamStorage += ">";
	
	// add new xml data to internal storage
	XmlStorage += tempStreamStorage;
	
	// return a value
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

