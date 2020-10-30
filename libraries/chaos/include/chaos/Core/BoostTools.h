#ifdef CHAOS_FORWARD_DECLARATION


#else 

namespace chaos
{
	namespace BoostTools
	{
		/** returns a string to be used as a name for resources */
		std::string PathToName(boost::filesystem::path const & path);

		/** returns (if possible) the absolute path. Use the reference_path to know about the root directory */
		boost::filesystem::path FindAbsolutePath(boost::filesystem::path const & reference_path, boost::filesystem::path const & path);

	}; // namespace BoostTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION