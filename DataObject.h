#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <future>

using SubLineInfo = std::tuple<size_t, size_t, std::string, bool>;//номер строки, позиция в строке, найденный текст, необходимость удаления

class DataObject
{
	size_t mCount;
	std::vector<SubLineInfo> mLines;
	std::vector<std::shared_future<void>> futures;

	void findSubLinesByMask(const std::string mask, const size_t ifirst, const size_t ilast);
public:
	DataObject();
	~DataObject();
	DataObject(const DataObject& obj);
	DataObject& operator=(const DataObject& obj);
	DataObject operator+(const DataObject& obj);

	void addSubObject(const size_t lineNumber, const size_t linePos, const std::string lineText, const bool willDelete);
	void printAllLines();
	size_t getCount() const;
	void addOneToCount();
	void startCompute(const std::string mask);
	void stopCompute();
	void deleteTrueLines();
};