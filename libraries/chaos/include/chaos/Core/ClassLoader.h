#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class ClassLoader;

}; // namespace chaos

#else 

namespace chaos
{
	class ClassLoader
	{
	public:

		/** load one class by its path */
		virtual Class const * LoadClass(FilePathParam const& path);
		/** load all classes in one directory */
		virtual bool LoadClassesInDirectory(FilePathParam const& path);
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


