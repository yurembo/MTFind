#include <iostream>

#include "DataObject.h"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/algorithm/string/replace.hpp>

size_t readData(boost::interprocess::mapped_region& region, DataObject& obj);

int main(int argc, char* argv[])
{
    if (argc != 3) return -1;//выходим, если не 3 параметра
	std::cout.setf(std::ios::boolalpha);
	std::string fileName = argv[1];
	std::string mask = argv[2];

	std::size_t s = mask.find('\n');
	if (s != std::string::npos) return -1;//выходим, если в маске присутствует символ конца строки - \n

	boost::algorithm::replace_all(mask, "?", ".");

	const boost::interprocess::mode_t mode = boost::interprocess::read_only;
	boost::interprocess::file_mapping fm(fileName.c_str(), mode);//открываем файл
	boost::interprocess::mapped_region fullRegion(fm, mode);//проецируем содержимое файла

	DataObject obj;
	readData(fullRegion, obj);
	obj.startCompute(mask);
	obj.stopCompute();
	obj.deleteTrueLines();
	std::cout << obj.getCount() << std::endl;
	obj.printAllLines();	
}

size_t readData(boost::interprocess::mapped_region& region, DataObject& obj)
{
	const char* first = static_cast<const char*>(region.get_address());
	auto size = region.get_size();
	auto last = first + size;
	size_t numLine = 0;//номер строки

	while (first && first < last)
	{
		auto middle = std::find(first, last, '\n');
		if (middle <= last)
		{
			auto mid = middle - first;
			std::string line = "";
			std::copy_n(first, mid, std::back_inserter(line));
			numLine++;
			first = middle;
			first++;

			obj.addOneToCount();
			obj.addSubObject(numLine, 0, line, false);
		}
	}
	return numLine;
}