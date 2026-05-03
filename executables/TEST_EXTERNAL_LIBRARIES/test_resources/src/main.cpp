#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <boost/filesystem/path.hpp>

int main(int argc, char ** argv)
{
	if (argc == 0)
		return -1;

	boost::filesystem::path application_path = argv[0];
	boost::filesystem::path application_directory = application_path.parent_path();
	boost::filesystem::path file_path = application_directory / "resources" / "Hello.txt";

	std::ifstream file(file_path.c_str());
	if (file)
	{
		std::streampos start = file.tellg();
		file.seekg(0, std::ios::end);
		std::streampos end = file.tellg();
		file.seekg(0, std::ios::beg);

		size_t file_size = (size_t)(end - start);

		std::cout << "file_path: " << file_path.string() << std::endl;
		std::cout << "file_size: " << file_size << std::endl;
		std::cout << "file_content: " << std::endl;

		std::string str;
		while (std::getline(file, str))
			std::cout << "  " << str << std::endl;
	}
	return 0;
}
 