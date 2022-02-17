namespace chaos
{
/**
* Note on file redirection:
*
*   The projects are split into 2 parts:
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
*   premake5.lua defines some helpful macro for that.
*
*   You can prevent this in _DEBUG with the command swith "-NoDirectResourceFiles"
*
*   You can only use this features if 'CHAOS_PROJECT_SRC_PATH' and 'CHAOS_PROJECT_BUILD_PATH' are defined (and in _DEBUG)
*
*   Beware do not use this macro in CHAOS library because it would be related to the library path.
*   Use it in EXE code instead
*/

#ifdef CHAOS_FORWARD_DECLARATION

	enum class LoadFileFlag;
	CHAOS_DECLARE_ENUM_FLAG_METHOD(LoadFileFlag);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

/**
* FileTools is namespace-class for methods to handle files
*/

	enum class LoadFileFlag // depending on the function, some flags may not be relevant
	{
		NONE = 0,
		ASCII = 1,
		NO_ERROR_TRACE = 2,
		RECURSIVE = 4
	};

	namespace FileTools
	{
		/** returns true if the extension of a file correspond to a string */
		bool IsTypedFile(FilePathParam const& path, char const* expected_ext);
		/** loading a whole file into memory */
		Buffer<char> LoadFile(FilePathParam const& path, LoadFileFlag flags = LoadFileFlag::NONE);

		/** try path redirection and call func (until it returns true) */
		bool ForEachRedirectedPath(FilePathParam const& path, std::function<bool(boost::filesystem::path const& p)> func);
		/** iterate over all entries in all possible directories (until func returns true) */
		bool ForEachRedirectedDirectoryContent(FilePathParam const& path, std::function<bool(boost::filesystem::path const& p)> func);

		/** returns a filepath that is unused */
		boost::filesystem::path GetUniquePath(FilePathParam const& path, char const* format, bool create_empty_file, int max_iterations = -1);

		/** create a temporary directory */
		bool CreateTemporaryDirectory(char const* pattern, boost::filesystem::path& result);

		/** read file as a vector of strings */
		std::vector<std::string> ReadFileLines(FilePathParam const& path, LoadFileFlag flags = LoadFileFlag::NONE);
		/** write a file with a vector of strings */
		bool WriteFileLines(FilePathParam const& path, std::vector<std::string> const& lines);

		/** redirect any access (under conditions) to the direct resources path of the project (not the build directory) */
#if _DEBUG
		bool GetRedirectedPath(boost::filesystem::path const& p, boost::filesystem::path const& build_path, boost::filesystem::path const& src_path, boost::filesystem::path& redirected_path);
#endif


	}; // namespace FileTools

#endif

}; // namespace chaos