namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename CPP_TYPE>
	class Class;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ClassSuperClass: utility meta class to get the inheritance from a Class<>
	 */

	template<typename CPP_TYPE>
	struct ClassSuperClass
	{
		using type = ClassBase;
	};

	template<typename CPP_TYPE>
	requires
	(
		HasSuperType<CPP_TYPE>
	)
	struct ClassSuperClass<CPP_TYPE>
	{
		using type = Class<SuperClass_t<CPP_TYPE>>;
	};

	template<typename CPP_TYPE>
	using ClassSuperClass_t = typename ClassSuperClass<CPP_TYPE>::type;

	/**
	 * Class: represent a class. can be used as a factory to generate elements 
	 */

	template<typename CPP_TYPE>
	class Class : public ClassSuperClass_t<CPP_TYPE>
	{
		friend class ClassManager;
		friend class NativeClassManager;

		template<typename CPP_TYPE>
		friend class CppClassRegisterResult;

	public:

		using Super = ClassSuperClass_t<CPP_TYPE>;

		/** create an instance of the described class */
		virtual CPP_TYPE* CreateInstance(void* inplace_buffer = nullptr) const
		{
			assert(this->IsFullyInitialized());
			CPP_TYPE* result = AllocateInstance(inplace_buffer);
			if (result != nullptr)
				this->InitializeInstance(result);
			return result;
		}
		/** get the singleton for native c++ classes */
		static Class const* GetNativeClassInstance()
		{
			return GetMutableNativeClassInstance();
		}
		/** generate a derived class from this one by adding special initialization handler */
		virtual CppClassRegisterResult<CPP_TYPE> CreateSubclass(ClassManager* in_manager, std::string in_name, std::function<void(CPP_TYPE*)> in_func = {}) const;

	protected:

		/** protected constructor */
		Class() = default;

		/** get the singleton for native c++ classes (mutable getter) */
		static Class* GetMutableNativeClassInstance()
		{
			static shared_ptr<Class<CPP_TYPE>> result = new Class<CPP_TYPE>;
			return result.get();
		}

		/** instance allocation method (does not apply initializer) */
		virtual CPP_TYPE* AllocateInstance(void* inplace_buffer) const
		{
			if (inplace_buffer == nullptr)
				return new CPP_TYPE;
			return new (inplace_buffer) CPP_TYPE;
		}

		/** internal method to create a child class from this one */
		virtual Class<CPP_TYPE>* CreateChildClass(ClassManager* in_manager, std::string in_name) const;
	};

#else

	template<typename CPP_TYPE>
	Class<CPP_TYPE>* Class<CPP_TYPE>::CreateChildClass(ClassManager* in_manager, std::string in_name) const
	{
		if (in_manager != nullptr)
		{
			// search if target manager is a child manager
			// (bad idea to have an ancestor manager having a derived class from one's child manager)
			if (this->manager != nullptr)
			{
				if (in_manager->IsAncestorManagerFor(this->manager, false))
				{
					ClassLog::Error("Can't create a child class in an ancestor ClassManager");
					return nullptr;
				}
			}
			// search if there already is a class with same name
			if (in_manager->FindClass(in_name.c_str(), FindClassFlags::Name | FindClassFlags::ParentManager) != nullptr) // ignore short name
			{
				ClassLog::Error("Can't load class [%s]: already existing in manager", in_name);
				return nullptr;
			}
		}

		Class<CPP_TYPE>* result = new Class<CPP_TYPE>();
		if (result == nullptr)
			return nullptr;

		result->name = in_name;
		result->class_size = this->class_size;
		result->info = this->info;
		result->parent_class = this;
		result->manager = in_manager;

		if (in_manager != nullptr)
			in_manager->InsertClass(result);
		return result;
	};

	template<typename CPP_TYPE>
	CppClassRegisterResult<CPP_TYPE> Class<CPP_TYPE>::CreateSubclass(ClassManager* in_manager, std::string in_name, std::function<void(CPP_TYPE*)> in_func) const
	{
		Class<CPP_TYPE>* result = CreateChildClass(in_manager, std::move(in_name));
		if (result == nullptr)
			return {};

		if (in_func)
		{
			result->initialize_instance_func = [func = std::move(in_func)](void* instance)
				{
					func((CPP_TYPE*)instance);
				};
		}
		return { result };
	}

#endif

}; // namespace chaos