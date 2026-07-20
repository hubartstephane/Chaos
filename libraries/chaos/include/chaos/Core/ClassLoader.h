namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ClassLoader: utility class used to load into its manager some class from one or more files
	 */

	class CHAOS_API ClassLoader
	{
	public:

		/** load one class by its path */
		virtual Class<Object>* LoadClass(ClassManager* manager, FilePathParam const& path) const;
		/** load all classes in one directory */
		virtual bool LoadClassesInDirectory(ClassManager* manager, FilePathParam const& path) const;

	protected:

		/* an utility function to load a JSON file a find the approriate classname */
		bool DoLoadClassHelper(FilePathParam const& path, LightweightFunction<bool(std::string, std::string, std::string, nlohmann::json)> func) const;
		/** create a child class */
		Class<Object>* DoCreateChildClass(ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json, char const* parent_class_name) const;
		/** create a child class */
		Class<Object>* DoCreateChildClassHelper(Class<Object>* parent_class, ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json) const;
	};

#endif

}; // namespace chaos