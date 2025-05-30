namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InheritanceType;

	class Class;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * CHAOS_REGISTER_CLASS : a macro that helps register classes automatically
	 */

#define CHAOS_REGISTER_CLASS(CLASS, ...) static inline chaos::Class const * CLASS##_class = chaos::ClassManager::GetDefaultInstance()->DeclareCPPClass<CLASS, __VA_ARGS__>(#CLASS)

	/**
	 * InheritanceType : the kind if inheritance that can exist between 2 classes
	 */

	enum class CHAOS_API InheritanceType : int
	{
		UNKNOWN = -1,
		NO = 0,
		YES = 1
	};

	/**
	 * Class : a registered class
	 */
	class CHAOS_API Class
	{
		friend class ClassLoader;
		friend class ClassRegistration;
		friend class ClassFindResult;
		friend class ClassManager;

	protected:

		/** constructor */
		Class(std::string in_name);

	public:

		/** the type of the function to allocate an object */
		using create_instance_function_type = std::function<Object* ()>;
		/** the type of the function to create an object on the stack an call a function on it */
		using create_instance_on_stack_function_type = std::function<void(LightweightFunction<void(Object*)>)>;

		/** destructor */
		virtual ~Class() = default;

		/** method to create an instance of the object */
		Object* CreateInstance() const;

		/** creation operator */
		Object* operator ()() const;

		/** create a temporary instance on the stack an call the functor on it */
		template<typename FUNC>
		bool CreateInstanceOnStack(FUNC const& func) const;

		/** gets the class size */
		size_t GetClassSize() const { return class_size; }
		/** gets the parent class */
		Class const* GetParentClass() const { return parent; }
		/** gets the class name */
		std::string const& GetClassName() const { return name; }
		/** gets the short name */
		std::string const& GetShortName() const { return short_name; }
		/** returns whether we can create instances */
		bool CanCreateInstance() const;
		/** returns whether we can create instances */
		bool CanCreateInstanceOnStack() const;
		/** gets the class manager */
		ClassManager* GetClassManager() const { return manager; } // no need to have a manager const

		/** static inheritance method */
		static InheritanceType InheritsFrom(Class const* child_class, Class const* parent_class, bool accept_equal = false);
		/** returns whether the class inherits from parent */
		InheritanceType InheritsFrom(Class const* parent_class, bool accept_equal = false) const;

		/** set the parent class */
		void SetParentClass(Class const * in_parent);
		/** set the short name */
		void SetShortName(std::string in_short_name);
		/** returns whether the class has been registered */
		bool IsDeclared() const;

	protected:

#if _DEBUG
		/** search whether the some parent class is in a child manager */
		bool HasCyclicParent() const;
#endif // #if _DEBUG

		/** object initialization function */
		virtual void OnObjectInstanceInitialized(Object* object) const;

		/** initialize the object instance */
		void InitializeObjectInstance(Object* object) const;
		/** get the create instance function */
		create_instance_function_type const* GetCreateInstanceFunc() const;
		/** get the create instance on stack function */
		create_instance_on_stack_function_type const* GetCreateInstanceOnStackFunc() const;

	protected:

		/** the parent of the class */
		Class const* parent = nullptr;
		/** get class size */
		size_t class_size = 0;
		/** the optional name of the class */
		std::string name;
		/** the optional short name of the class */
		std::string short_name;
		/** whether the class has been fully declared */
		bool declared = false;
		/** create an instance of the object delegate */
		create_instance_function_type create_instance;
		/** delegate to create a temporary instance of the object on the stack and call a functor on it */
		create_instance_on_stack_function_type create_instance_on_stack;
		/** the type_info for the class */
		std::type_info const* info = nullptr;
		/** the manager for this class */
		ClassManager* manager = nullptr;
	};

#else

CHAOS_DEFINE_LOG(ClassLog, "Class")

template<typename FUNC>
bool Class::CreateInstanceOnStack(FUNC const& func) const
{
	if (create_instance_on_stack_function_type const* create_func = GetCreateInstanceOnStackFunc())
	{
		(*create_func)([this, &func](Object* object)
			{
				InitializeObjectInstance(this, object);
				func(auto_cast_checked(object));
			});
		return true;
	}
	else
	{
		ClassLog::Error("Class::CreateInstanceOnStack : the class [%s] cannot be instanciated", name.c_str());
		return false;
	}
}

#endif

}; // namespace chaos