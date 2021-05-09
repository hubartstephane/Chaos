namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class ClassLoader
	{
	public:

		/** load one class by its path */
		virtual Class const* LoadClass(FilePathParam const& path);
		/** load all classes in one directory */
		virtual bool LoadClassesInDirectory(FilePathParam const& path);
	};

#endif

}; // namespace chaos