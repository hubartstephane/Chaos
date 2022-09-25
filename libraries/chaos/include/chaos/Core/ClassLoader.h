namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API ClassLoader
	{
	public:

		/** load one class by its path */
		virtual Class * LoadClass(ClassManager * manager, FilePathParam const& path);
		/** load all classes in one directory */
		virtual bool LoadClassesInDirectory(ClassManager* manager, FilePathParam const& path);

	protected:

		/** internal method to declare a class without finding yet its parent (used for directory iteration) */
		Class* DoDeclareSpecialClassCreate(ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json); // XXX : no const return value here !! (for Finalization of special class)
		/** finalization of a special class (called from ClassLoader) : find parent */
		bool DoDeclareSpecialClassPatchParent(ClassManager* manager, Class* cls, std::string const & parent_class_name);
		/** declare a pseudo class, that is a class with additionnal json initialization */
		Class * DeclareSpecialClass(ClassManager* manager, std::string class_name, std::string short_name, std::string const & parent_class_name, nlohmann::json json);

		/** internal method called from ClassLoader to abord a failed loaded class */
		 void DoInvalidateSpecialClass(ClassManager* manager, Class const* cls);
	};

#endif

}; // namespace chaos