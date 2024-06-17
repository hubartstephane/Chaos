namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassManager;

	enum class FindClassFlags;
	CHAOS_DECLARE_ENUM_BITMASK_METHOD(FindClassFlags, CHAOS_API);

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION


	/**
	 * ClassManager: an object that registered some classes, C++ or JSON
	 */

	enum class FindClassFlags : int
	{
		NAME = 1,             // consider name when searching
		SHORTNAME = 2,        // consider shortname when searching
		PARENT_MANAGER = 4,   // search into child managers
		ALL = NAME | SHORTNAME | PARENT_MANAGER
	};

	/**
	 * ClassManager: an object that registered some classes, C++ or JSON
	 */

	class CHAOS_API ClassManager : public Object
	{
	public:

		friend class ClassLoader;
		friend class ClassRegistration;
		friend class ClassFindResult;
		friend class Class;

		/** constructor */
		ClassManager(ClassManager* in_parent_manager = nullptr);

		/** getters on the default manager */
		static ClassManager* GetDefaultInstance();

		/** gets the parent manager */
		ClassManager* GetParentManager() { return parent_manager.get(); }
		/** gets the parent manager */
		ClassManager const * GetParentManager() const { return parent_manager.get(); }

		/** find a class by name */
		ClassFindResult FindClass(char const* name, FindClassFlags flags = FindClassFlags::ALL);

		/** find a class by type */
		template<typename CLASS_TYPE>
		Class const* FindCPPClass(bool search_manager_hierarchy = true)
		{
			if (Class* result = FindOrCreateCPPClassInstance<CLASS_TYPE>(search_manager_hierarchy))
				if (result->IsDeclared())
					return result;
			return nullptr;
		}

		/** declare a class */
		template<typename CLASS_TYPE, typename PARENT_CLASS_TYPE = EmptyClass>
		ClassRegistration DeclareCPPClass(std::string name)
		{
			assert((std::is_same_v<PARENT_CLASS_TYPE, EmptyClass> || std::is_base_of_v<PARENT_CLASS_TYPE, CLASS_TYPE>));
			assert(!StringTools::IsEmpty(name));

			Class* result = FindOrCreateCPPClassInstance<CLASS_TYPE>(false);
			if (result != nullptr)
			{
				// already declared, returns (this may happen with multiple .h inclusions)
				if (result->declared)
					return { result };

				result->name = std::move(name);
				result->class_size = sizeof(CLASS_TYPE);
				result->declared = true;
				result->info = &typeid(CLASS_TYPE);

				// instance constructible only if derives from Object
				if constexpr (std::is_base_of_v<Object, CLASS_TYPE>)
				{
					result->create_instance = []()
					{
						return new CLASS_TYPE;
					};

					result->create_instance_on_stack = [](LightweightFunction<void(Object*)> func)
					{
						CLASS_TYPE instance;
						func(&instance);
					};
				}
				// the parent is accessed, but not necesseraly initialized yet
				if (!std::is_same_v<PARENT_CLASS_TYPE, EmptyClass>)
					result->parent = FindOrCreateCPPClassInstance<PARENT_CLASS_TYPE>(true);
			}
			return { result };
		}

	protected:

		/** return the class of a class with its given info */
		template<typename CLASS_TYPE>
		Class* FindOrCreateCPPClassInstance(bool search_manager_hierarchy)
		{
			std::type_info const& info = typeid(CLASS_TYPE);

			// search if the class as already been registered in manager chain
			ClassManager* manager = this;
			while (manager != nullptr)
			{
				for (Class* cls : manager->classes)
					if (cls->info != nullptr && *cls->info == info)
						return cls;
				if (!search_manager_hierarchy)
					break;
				manager = manager->parent_manager.get();
			}
			// register the class
			if (Class* result = new Class(this, std::string()))
			{
				result->info = &info;
				return result;
			}
			return nullptr;
		}

	protected:

		/* the parent class manager */
		shared_ptr<ClassManager> parent_manager;
		/** the classes owned by this manager */
		std::vector<Class*> classes;
	};

#endif

}; // namespace chaos