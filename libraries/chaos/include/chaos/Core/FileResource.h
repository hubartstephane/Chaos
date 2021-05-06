#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class FileResource;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{
	/** an object that have a path and a timestamp */
	class FileResource
	{
	public:

		/** get the path of the object */
		boost::filesystem::path const & GetPath() const { return path; }
		/** Set the path method (for friends only) */
		void SetPath(boost::filesystem::path const& in_path);

		/** get the file last write time */
		std::time_t GetFileTimestamp() const { return file_timestamp; }

	protected:

		/** the path of the object */
		boost::filesystem::path path;
		/** the file timestamp */
		std::time_t file_timestamp = 0;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
