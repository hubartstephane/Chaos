#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace FileTools
	{

	}; // namespace FileTools

}; // namespace chaos

#else 

namespace chaos
{
	/**
	* FileTools is namespace-class for methods to handle files
	*/

	namespace FileTools
	{
		/** returns true if the extension of a file correspond to a string */
		bool IsTypedFile(FilePathParam const & path, char const * expected_ext);
		/** loading a whole file into memory */
		Buffer<char> LoadFile(FilePathParam const & path, bool ascii, bool * success_open = nullptr);

		/** returns an iterator over a directory (can use resource direct access) */
		boost::filesystem::directory_iterator GetDirectoryIterator(FilePathParam const & path);
		/** returns a filepath that is unused */
		boost::filesystem::path GetUniquePath(FilePathParam const & path, char const * format, bool create_empty_file, int max_iterations = -1);

		/** create a temporary directory */
		bool CreateTemporaryDirectory(char const * pattern, boost::filesystem::path & result);

		/** read file as a vector of strings */
		std::vector<std::string> ReadFileLines(FilePathParam const & path, bool * success_open = nullptr);
		/** write a file with a vector of strings */
		bool WriteFileLines(FilePathParam const & path, std::vector<std::string> const & lines);

		/** redirect any access (under conditions) to the direct resources path of the project (not the build directory) */
#if CHAOS_CAN_REDIRECT_RESOURCE_FILES
		bool GetRedirectedPath(boost::filesystem::path const & p, boost::filesystem::path & redirected_path);
#endif

	}; // namespace FileTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

