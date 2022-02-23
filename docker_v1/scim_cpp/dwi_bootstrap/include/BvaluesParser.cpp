/*
 * BvaluesParser.cpp
 *
 *  Created on: Jan 13, 2011
 *      Author: ch145864
 */

#include "BvaluesParser.h"
#include <fstream>
#include <iostream>

BvaluesParser::BvaluesParser(const int bValuesNum, const std::string & filename) :
_bValuesNum (bValuesNum),
_bValues (),
_bValueImageNames ()
{
	std::fstream inFile;
	inFile.open (filename.c_str(), std::ios::in);

	double bValue;
	std::string bValFilename;

	for (int i=0;i<_bValuesNum;++i)
	{
		inFile >> bValue >> bValFilename;
		//std::cout << bValue << ": " << bValFilename << std::endl;
		_bValues.push_back(bValue);
		_bValueImageNames.push_back(bValFilename);
	}
	inFile.close();


}

