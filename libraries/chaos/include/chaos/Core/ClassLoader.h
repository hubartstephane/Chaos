namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API ClassLoader
	{
	public:

		/** load one class by its path */
		virtual Class const* LoadClass(FilePathParam const& path);
		/** load all classes in one directory */
		virtual bool LoadClassesInDirectory(FilePathParam const& path);

	protected:

		/** internal method to declare a class without finding yet its parent (used for directory iteration) */
		Class* DoDeclareSpecialClassStep1(std::string class_name, std::string short_name, nlohmann::json const& json); // XXX : no const return value here !! (for Finalization of special class)
		/** finalization of a special class (called from ClassLoader) : find parent */
		bool DoDeclareSpecialClassStep2(Class* cls);
		/** finalization of a special class (called from ClassLoader) : creation delegate */
		bool DoDeclareSpecialClassStep3(Class* cls);
		/** declare a pseudo class, that is a class with additionnal json initialization */
		Class const* DeclareSpecialClass(std::string class_name, std::string short_name, nlohmann::json const& json);

		/** internal method called from ClassLoader to abord a failed loaded class */
		 void DoInvalidateSpecialClass(Class const* cls);
	};

#endif

}; // namespace chaos