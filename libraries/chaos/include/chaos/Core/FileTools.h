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
*   You can only use this features if 'DEATH_PROJECT_SRC_PATH' and 'DEATH_PROJECT_BUILD_PATH' are defined (and in _DEBUG)
*
*   Beware do not use this macro in CHAOS library because it would be related to the library path.
*   Use it in EXE code instead
*/

#ifdef CHAOS_FORWARD_DECLARATION

	enum class LoadFileFlag;
	CHAOS_DECLARE_ENUM_FLAG_METHOD(LoadFileFlag, CHAOS_API);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

/**
* FileTools is namespace-class for methods to handle files
*/

	enum class CHAOS_API LoadFileFlag // depending on the function, some flags may not be relevant
	{
		NONE = 0,
		ASCII = 1,
		NO_ERROR_TRACE = 2,
		RECURSIVE = 4
	};

	namespace FileTools
	{
		/** returns true if the extension of a file correspond to a string */
		CHAOS_API bool IsTypedFile(FilePathParam const& path, char const* expected_ext);
		/** loading a whole file into memory */
		CHAOS_API Buffer<char> LoadFile(FilePathParam const& path, LoadFileFlag flags = LoadFileFlag::NONE);

		/** try path redirection and call func (until it returns true) */
		CHAOS_API bool WithFile(FilePathParam const& path, LightweightFunction<bool(boost::filesystem::path const& p)> func);
		/** iterate over all entries in all possible directories (until func returns true) */
		CHAOS_API bool WithDirectoryContent(FilePathParam const& path, LightweightFunction<bool(boost::filesystem::path const& p)> func);

		/** returns a filepath that is unused */
		CHAOS_API boost::filesystem::path GetUniquePath(FilePathParam const& path, char const* format, bool create_empty_file, int max_iterations = -1);

		/** create a temporary directory */
		CHAOS_API bool CreateTemporaryDirectory(char const* pattern, boost::filesystem::path& result);

		/** read file as a vector of strings */
		CHAOS_API std::vector<std::string> ReadFileLines(FilePathParam const& path, LoadFileFlag flags = LoadFileFlag::NONE);
		/** write a file with a vector of strings */
		CHAOS_API bool WriteFileLines(FilePathParam const& path, std::vector<std::string> const& lines);

		/** redirect any access (under conditions) to the direct resources path of the project (not the build directory) */
		CHAOS_API boost::filesystem::path GetRedirectedPath(boost::filesystem::path const& p);

	}; // namespace FileTools

#endif

}; // namespace chaos