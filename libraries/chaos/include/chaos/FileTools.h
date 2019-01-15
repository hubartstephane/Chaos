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

		/** returns true if the extension of a file correspond to a string */
		static bool IsTypedFile(FilePathParam const & path, char const * expected_ext);
		/** loading a whole file into memory */
		static Buffer<char> LoadFile(FilePathParam const & path, bool ascii, bool * success_open = nullptr);

		/** create a temporary directory */
		static bool CreateTemporaryDirectory(char const * pattern, boost::filesystem::path & result);

		/** read file as a vector of strings */
		static std::vector<std::string> ReadFileLines(FilePathParam const & path);
		/** write a file with a vector of strings */
		static bool WriteFileLines(FilePathParam const & path, std::vector<std::string> const & lines);

		/** redirect any access (under conditions) to the direct resources path of the project (not the build directory) */
#if CHAOS_CAN_REDIRECT_RESOURCE_FILES
		static bool GetRedirectedPath(boost::filesystem::path const & p, boost::filesystem::path & redirected_path);
#endif

	protected:

		/** an utility function to test for extension */
		static bool DoIsTypedFile(char const * filename, char const * expected_ext);
		/** an utility function to load a file */
		static Buffer<char> DoLoadFile(boost::filesystem::path const & resolved_path, bool ascii, bool * success_open = nullptr);
	};

}; // namespace chaos

