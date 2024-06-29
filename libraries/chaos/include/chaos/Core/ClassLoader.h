namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassWithJSONInitialization;
	class ClassLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API ClassWithJSONInitialization : public Class
	{
		friend class ClassLoader;

	protected:

		/** constructor */
		ClassWithJSONInitialization(std::string in_name, nlohmann::json in_json);

		/** override */
		virtual void OnObjectInstanceInitialized(Object* object) const override;

	protected:

		/** the data to apply at object instance creation */
		nlohmann::json json;
	};

	class CHAOS_API ClassLoader
	{
	public:

		/** load one class by its path */
		virtual Class * LoadClass(ClassManager * manager, FilePathParam const& path) const;
		/** load all classes in one directory */
		virtual bool LoadClassesInDirectory(ClassManager* manager, FilePathParam const& path) const;

	protected:

		/** internal method to declare a class without finding yet its parent (used for directory iteration) */
		Class* DoCreateSpecialClass(ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json) const; // XXX : no const return value here !! (for Finalization of special class)
		/** finalization of a special class (called from ClassLoader) : find parent */
		bool DoSetSpecialClassParent(ClassManager* manager, Class* cls, std::string const & parent_class_name) const;
		/** internal method called from ClassLoader to abord a failed loaded class */
		void DoDeleteSpecialClass(ClassManager* manager, Class * cls) const;
		/** internal method to complete the load */
		void DoCompleteSpecialClassMissingData(Class * cls) const;
	};

#endif

}; // namespace chaos