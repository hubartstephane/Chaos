#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Buffer.h>
#include <chaos/FilePath.h>

namespace chaos
{
	/**
	 * Note on file redirection:
	 *
	 *   The projects are split in 2 parts:
	 *     - the executable
	 *     - the resource files
	 *
	 *   Under normal circonstances, you have to 'build' the resource files (make a copy into to exe directory) in order to access them at runtime
	 *   As a side effect, every time you change one file in the resource project, you have to make a copy again of all files into exe directory
	 *   This prevent direct edition and further hot-reloading of resources
	 *
	 *   That why i added a redirect resources mecanim.
	 *   in DEBUG, we try to match files in BUILD_PATH and files in SRC_PATH to make a direct access from the SOURCE
	 *
	 *    premake5.lua defines some helpful macro for that. The command line has to have the swith "-DirectResourceFiles"
	 */


	/** whether the executable has all required macros defined to make direct access to resources files */
	#define CHAOS_CAN_REDIRECT_RESOURCE_FILES (_DEBUG && defined CHAOS_PROJECT_PATH && defined CHAOS_PROJECT_SRC_PATH && defined CHAOS_PROJECT_BUILD_PATH)


	/**
	* FileTools is namespace-class for methods to handle files
	*/

	class FileTools
	{
	public:

		/** returns true if the extension of a file correspond to a string */
		static bool IsTypedFile(FilePathParam const & path, char const * expected_ext);
		/** loading a whole file into memory */
		static Buffer<char> LoadFile(FilePathParam const & path, bool ascii);

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
		static Buffer<char> FileTools::DoLoadFile(boost::filesystem::path const & resolved_path, bool ascii, bool * success_open);
	};

}; // namespace chaos

