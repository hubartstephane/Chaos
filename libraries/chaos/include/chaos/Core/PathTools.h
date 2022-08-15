namespace chaos
{
	namespace PathTools
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** returns a string to be used as a name for resources */
		std::string PathToName(boost::filesystem::path const & path);

		/** returns (if possible) the absolute path. Use the reference_path to know about the root directory */
		boost::filesystem::path FindAbsolutePath(boost::filesystem::path const & reference_path, boost::filesystem::path const & path);

#endif

	}; // namespace PathTools

}; // namespace chaos