// xml.h - header file for functions related to git tree parsing and pulling

#ifndef _XML_H_
#define _XML_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

class XmlLeafProcessor {
	public:
		XmlLeafProcessor();
		~XmlLeafProcessor();
		int AddLeafToXml(int x, int y, int rotation);
		int GenerateXmlFile(std::string fileName);
	protected:
	private:	
		std::string XmlStorage;
}

#endif //_XML_H_
