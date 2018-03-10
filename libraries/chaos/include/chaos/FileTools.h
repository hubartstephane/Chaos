#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>
#include <chaos/FilePath.h>

namespace chaos
{

	/**
	* FileTools is namespace-class for methods to handle files
	*/

	class FileTools
	{
	public:
     
		/** returns a sub pointer on the extension of the file */
		static char const * GetFilenameExtension(char const * filename);
		/** returns true if the extension of a file correspond to a string */
		static bool IsTypedFile(char const * filename, char const * expected_ext);
		/** loading a whole file into memory */
		static Buffer<char> LoadFile(FilePath && path, bool ascii);

		/** create a temporary directory */
		static bool CreateTemporaryDirectory(char const * pattern, boost::filesystem::path & result);

		/** read file as a vector of strings */
		static std::vector<std::string> ReadFileLines(char const * filename);
		/** write a file with a vector of strings */
		static bool WriteFileLines(char const * filename, std::vector<std::string> const & lines);
	};

}; // namespace chaos

