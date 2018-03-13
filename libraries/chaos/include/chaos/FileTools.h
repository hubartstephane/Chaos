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
		static char const * GetFilenameExtension(FilePath const & path);
		/** returns true if the extension of a file correspond to a string */
		static bool IsTypedFile(FilePath const & path, char const * expected_ext);
		/** loading a whole file into memory */
		static Buffer<char> LoadFile(FilePath const & path, bool ascii);

		/** resolving the path */
		static boost::filesystem::path ResolvePath(FilePath const & path);

		/** create a temporary directory */
		static bool CreateTemporaryDirectory(char const * pattern, boost::filesystem::path & result);

		/** read file as a vector of strings */
		static std::vector<std::string> ReadFileLines(FilePath const & path);
		/** write a file with a vector of strings */
		static bool WriteFileLines(FilePath const & path, std::vector<std::string> const & lines);
	};

}; // namespace chaos

