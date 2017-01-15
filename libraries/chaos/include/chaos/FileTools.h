#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>

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
  static Buffer<char> LoadFile(char const * filename, bool ascii);
  /** loading a whole file into memory */
  static Buffer<char> LoadFile(boost::filesystem::path const & filename, bool ascii);
  /** create a temporary directory */
  static bool CreateTemporaryDirectory(char const * pattern, boost::filesystem::path & result);

	/** read file as a vector of strings */
	static std::vector<std::string> ReadFileLines(char const * filename);
};

}; // namespace chaos

