#include "DataObject.h"
#include <iostream>
#include <regex>
#include <iterator>
#include <algorithm>

const int gThreadsCount = 2;// число потоков

void DataObject::addSubObject(const size_t lineNumber, const size_t linePos, const std::string lineText, const bool willDelete)
{
	SubLineInfo	sli = std::make_tuple(lineNumber, linePos, lineText, willDelete);
	mLines.push_back(sli);
}

void DataObject::printAllLines()
{
	auto iter = mLines.begin();
	while (iter != mLines.end()) {
		SubLineInfo sli = SubLineInfo(*iter);
		std::string s = std::get<2>(sli);
		s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
		s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
		std::cout << std::get<0>(sli) << " " << std::get<1>(sli) << " " << s  << /*" " << '(' << std::get<3>(sli) << ')' <<*/ std::endl;
		++iter;
	}
}

size_t DataObject::getCount() const
{
	//return mCount;
	return mLines.size();
}

void DataObject::addOneToCount()
{
	++mCount;
}

void DataObject::startCompute(const std::string mask)
{
	size_t linesCount = mLines.size();
	size_t partLinesCount = linesCount / gThreadsCount;
	size_t rest = linesCount % gThreadsCount;
	
	for (int i = 0; i < gThreadsCount; ++i) {
		size_t upLinesCount = partLinesCount;
		if (i == gThreadsCount - 1) {
			upLinesCount = (i + 1) * partLinesCount + rest;
		}
		else {
			upLinesCount = (i + 1) * partLinesCount;
		}
		std::shared_future<void> future { std::async(std::launch::async, &DataObject::findSubLinesByMask, this, mask, i * partLinesCount, upLinesCount) };
		futures.push_back(future);
	}
}

void DataObject::stopCompute()
{
	for (int i = 0; i < gThreadsCount; ++i) {
		futures[i].get();
	}
}

void DataObject::findSubLinesByMask(const std::string mask, const size_t ifirst, const size_t ilast)
{
	if (!mLines.empty()) 
	{
		for (size_t i = ifirst; i < ilast; ++i) 
		{
			std::smatch sMatch;
			std::regex rMask{ mask }; 
			std::string subLine = std::get<2>(mLines[i]);

			if (std::regex_search(subLine, sMatch, rMask)) {
				size_t npos = subLine.find(sMatch.str());
				std::get<1>(mLines[i]) = npos + 1;
				std::get<2>(mLines[i]) = sMatch.str();
			}
			else {
				std::get<3>(mLines[i]) = true;
			}
		}
	}
}

void DataObject::deleteTrueLines()
{
	if (!mLines.empty())
		std::erase_if(mLines, [](SubLineInfo sli) { return std::get<3>(sli); });
}

DataObject::DataObject()
{
	mCount = 0;
	mLines.clear();
}

DataObject::DataObject(const DataObject& obj)
{
	mCount = obj.getCount();
	mLines = obj.mLines;
}

DataObject DataObject::operator+(const DataObject& obj)
{
	DataObject ob;
	ob.mCount = mCount + obj.mCount;
	ob.mLines.insert(std::end(ob.mLines), std::begin(mLines), std::end(mLines));
	ob.mLines.insert(std::end(ob.mLines), std::begin(obj.mLines), std::end(obj.mLines));
	return ob;
}

DataObject& DataObject::operator=(const DataObject& obj)
{
	if (&obj != this) {
		mCount = obj.mCount;
		mLines = obj.mLines;
	}
	return *this;
}

DataObject::~DataObject()
{
	mCount = 0;
	mLines.clear();
}