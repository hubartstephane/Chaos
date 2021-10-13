namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InheritanceType;

	class Class;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * CHAOS_REGISTER_CLASS : a macro that helps register classes automatically
	 */

	 // ideally this should be replaced with __VA_OPT__, but not supported yet by VS2019

#define CHAOS_REGISTER_CLASS1(classname) inline chaos::Class const * classname##_class = chaos::Class::DeclareClass<classname>(#classname);
#define CHAOS_REGISTER_CLASS2(classname, parent_classname) inline chaos::Class const * classname##_class = chaos::Class::DeclareClass<classname, parent_classname>(#classname);

	/**
	 * InheritanceType : the kind if inheritance that can exist between 2 classes
	 */

	enum class InheritanceType : int
	{
		UNKNOWN = -1,
		NO = 0,
		YES = 1
	};

	/**
	 * Class : a registered class
	 */
	class Class
	{
		friend class ClassLoader;

	public:

		/** method to create an instance of the object */
		Object* CreateInstance() const;
		/** create a temporary instance on the stack an call the functor on it */
		bool CreateInstanceOnStack(std::function<void(Object*)> func) const;
		/** create a temporary instance on the stack an call the functor on it */
#if 0
		template<typename T, typename RESULT_TYPE>
		RESULT_TYPE CreateInstanceOnStack2(std::function<RESULT_TYPE(T*)> func) const
		{
			static_assert(std::is_base_of_v<Object, T>);

			RESULT_TYPE result;
			CreateInstanceOnStack([func, &result](Object * object)
			{
				result = func(auto_cast(object));
			});
			return result;
		}
#endif

		template<typename T>
		void CreateInstanceOnStack2(std::function<void(T*)> func) const
		{
			static_assert(std::is_base_of_v<Object, T>);

			CreateInstanceOnStack([func](Object * object)
			{
				func(auto_cast(object));
			});
		}

		/** returns whether the class has been registered */
		bool IsDeclared() const;
		/** gets the class size */
		size_t GetClassSize() const { return class_size; }
		/** gets the parent class */
		Class const* GetParentClass() const { return parent; }
		/** gets the class name */
		std::string const& GetClassName() const { return class_name; }
		/** returns whether we can create instances */
		bool CanCreateInstance() const { return create_instance_func != nullptr; }
		/** returns whether we can create instances */
		bool CanCreateInstanceOnStack() const { return create_instance_on_stack_func != nullptr; }


	public:

		/** find a class by name */
		static Class const* FindClass(char const* class_name);

		/** find a class by type */
		template<typename CLASS_TYPE>
		static Class const* FindClass()
		{
			Class const* result = GetClassInstance<CLASS_TYPE>();
			if (!result->IsDeclared())
				return nullptr;
			return result;
		}

		/** declare a class */
		template<typename CLASS_TYPE, typename PARENT_CLASS_TYPE = EmptyClass>
		static Class const* DeclareClass(char const* class_name)
		{
			// check parameter and not already registered
			bool empty_name = StringTools::IsEmpty(class_name);

			assert(empty_name || FindClass(class_name) == nullptr);
			assert((std::is_same_v<PARENT_CLASS_TYPE, EmptyClass> || std::is_base_of_v<PARENT_CLASS_TYPE, CLASS_TYPE>));

			Class* result = GetClassInstance<CLASS_TYPE>();
			if (result != nullptr)
			{
				// do not register class whose name is empty
				if (!empty_name)
				{
					GetClassesList().push_back(result);
					result->class_name = class_name;
				}
				result->class_size = sizeof(CLASS_TYPE);
				result->declared = true;
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
					result->parent = GetClassInstance<PARENT_CLASS_TYPE>();
			}
			return result;
		}
		/** static inheritance method */
		static InheritanceType InheritsFrom(Class const* child_class, Class const* parent_class, bool accept_equal = false);
		/** returns whether the class inherits from parent */
		InheritanceType InheritsFrom(Class const* parent_class, bool accept_equal = false) const;

	protected:

		/** return the class for a type even if not initialized */
		template<typename CLASS_TYPE>
		static Class* GetClassInstance()
		{
			static Class* result = new Class;
			return result;
		}

		/** get the list of all classes */
		static std::vector<Class*>& GetClassesList();

	protected:

		/** the parent of the class */
		Class const* parent = nullptr;
		/** get class size */
		size_t class_size = 0;
		/** the optional name of the class */
		std::string class_name;
		/** whether the class has been fully declared */
		bool declared = false;
		/** create an instance of the object delegate */
		std::function<Object* ()> create_instance_func;
		/** delegate to create a temporary instance of the object on the stack and call a functor on it */
		std::function<void(std::function<void(Object*)>)> create_instance_on_stack_func;
		/** additionnal initialization for JSONSerializable objects */
		nlohmann::json json_data;
	};

#endif

}; // namespace chaos