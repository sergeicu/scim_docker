/*
 * BvaluesParser.h
 *
 *  Created on: Jan 13, 2011
 *      Author: ch145864
 */

#ifndef BVALUESPARSER_H_
#define BVALUESPARSER_H_

#include <string>
#include <vector>

class BvaluesParser
{
public:
	BvaluesParser() : _bValuesNum (0), _bValues (), _bValueImageNames () {}
	BvaluesParser(const int bValuesNum, const std::string & filename);

	~BvaluesParser() {};

	typedef std::vector <double> BValuesVecType;
	typedef std::vector <std::string> BValuesFileNamesVecType;

	BValuesVecType getBValues () {return _bValues;}
	std::string getBValueImageName (int i) {return _bValueImageNames[i];}


private:
	int _bValuesNum;
	BValuesVecType _bValues;
	BValuesFileNamesVecType _bValueImageNames;


};

#endif /* BVALUESPARSER_H_ */
