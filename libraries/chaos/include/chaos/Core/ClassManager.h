namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassManager;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ClassManager: an object that registered some classes, C++ or JSON
	 */

	class CHAOS_API ClassManager// : public Object
	{
	public:

		friend class ClassLoader;
		friend class ClassRegistration;
		friend class ClassFindResult;

		/** constructor */
		ClassManager(ClassManager* in_parent_manager = nullptr);

		/** getters on the default manager */
		static ClassManager* GetDefaultInstance();

		/** gets the parent manager */
		//ClassManager* GetParentManager() { return parent_manager.get(); }

		ClassManager* GetParentManager() { return parent_manager; }
		/** gets the parent manager */
		//ClassManager const * GetParentManager() const { return parent_manager.get(); }

		ClassManager const* GetParentManager() const { return parent_manager; }

		/** find a class by name */
		ClassFindResult FindClass(char const* name);

		/** find a class by type */
		template<typename CLASS_TYPE>
		Class const* FindClass()
		{
			if (Class* result = FindOrCreateClassInstance<CLASS_TYPE>())
				if (result->IsDeclared())
					return result;
			return nullptr;
		}

		/** declare a class */
		template<typename CLASS_TYPE, typename PARENT_CLASS_TYPE = EmptyClass>
		ClassRegistration DeclareClass(std::string name)
		{
			assert((std::is_same_v<PARENT_CLASS_TYPE, EmptyClass> || std::is_base_of_v<PARENT_CLASS_TYPE, CLASS_TYPE>));
			assert(!StringTools::IsEmpty(name));

			Class* result = FindOrCreateClassInstance<CLASS_TYPE>();
			if (result != nullptr)
			{
				// already declared, returns (this may happens with multiple .h inclusions)
				if (result->declared)
					return result;

				result->name = std::move(name);
				result->class_size = sizeof(CLASS_TYPE);
				result->declared = true;
				result->info = &typeid(CLASS_TYPE);

				// instance constructible only if derives from Object
				if constexpr (std::is_base_of_v<Object, CLASS_TYPE>)
				{
					result->create_instance_func = []()
					{
						return new CLASS_TYPE;
					};

					result->create_instance_on_stack_func = [](std::function<void(Object*)> func)
					{
						CLASS_TYPE instance;
						func(&instance);
					};
				}
				// the parent is accessed, but not necessaraly initialized yet
				if (!std::is_same_v<PARENT_CLASS_TYPE, EmptyClass>)
					result->parent = FindOrCreateClassInstance<PARENT_CLASS_TYPE>();
			}
			return { result };
		}

	protected:

		/** return the class of a class with its given info */
		template<typename CLASS_TYPE>
		Class* FindOrCreateClassInstance()
		{
			std::type_info const& info = typeid(CLASS_TYPE);

			// search if the class as already been registered in manager chain
			ClassManager* manager = this;
			while (manager != nullptr)
			{
				for (Class* cls : manager->classes)
					if (*cls->info == info)
						return cls;
				manager = manager->parent_manager;
			}
			// register the class
			Class * result = new Class;
			result->info = &info;
			result->manager = this;
			classes.push_back(result);
			return result;
		}

	protected:

		/* the parent class manager */
		//shared_ptr<ClassManager> parent_manager;

		ClassManager* parent_manager = nullptr;
		/** the classes owned by this manager */
		std::vector<Class*> classes;
	};

#endif

}; // namespace chaos