namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassManager;
	class ClassRegistration;
	class ClassFindResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ClassRegistration : the kind if inheritance that can exist between 2 classes
	 */

	class CHAOS_API ClassRegistration
	{
		friend class ClassManager;

	protected:

		/** constructor (may only be called from Class) */
		ClassRegistration(Class* in_class_ptr) :
			class_ptr(in_class_ptr)
		{
			assert(in_class_ptr != nullptr);
		}

	public:

		/** set the short name for the class */
		ClassRegistration & operator()(std::string in_short_name);

		/** convert the registration to the class */
		operator Class const * () const
		{
			return class_ptr;
		}

	protected:

		/** the class */
		Class * class_ptr = nullptr;
	};

	/**
	 * ClassFindResult : Intermediate object for class searching.
	 *                   While there may be several classes with the same short_name, we have to select the good one when searching
	 *                   We so want that the search is affected by the Subclass affectation that requires it.
	 */

	class CHAOS_API ClassFindResult
	{
		friend class ClassManager;

		using short_name_iterator_type = std::vector<Class*>::iterator;

	public:

		/** gets the result of the class */
		operator Class* () const;
		/** finalize the search */
		Class* Resolve(Class const* check_class) const;

	protected:

		/** constructor */
		ClassFindResult(Class* in_result, ClassManager * in_class_manager, short_name_iterator_type in_short_name_iterator);

		/** if the result of the search is found by class name, directly store this here */
		Class * result = nullptr;
		/** the class manager where to search */
		ClassManager* class_manager = nullptr;
		/** the very first short name matching the request. we can use it for further reserch instead to store the name somehow (that would be costly) */
		short_name_iterator_type short_name_iterator;
	};

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
					result->create_instance_func = []() { return new CLASS_TYPE; };

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
			// search if the class as already been registered
			std::type_info const& info = typeid(CLASS_TYPE);
			for (Class* cls : classes)
				if (*cls->info == info)
					return cls;
			// register the class
			static Class cls;
			cls.info = &info;
			classes.push_back(&cls);
			return &cls;
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