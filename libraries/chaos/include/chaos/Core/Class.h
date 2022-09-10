namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InheritanceType;

	class Class;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * CHAOS_REGISTER_CLASS : a macro that helps register classes automatically
	 */

#define CHAOS_REGISTER_CLASS(CLASS, ...) static inline chaos::Class const * CLASS##_class = chaos::ClassManager::GetDefaultInstance()->DeclareClass<CLASS, __VA_ARGS__>(#CLASS)

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

	public:

		/** method to create an instance of the object */
		Object* CreateInstance() const;
		/** create a temporary instance on the stack an call the functor on it */
		template<typename FUNC>
		bool CreateInstanceOnStack(FUNC func) const
		{
			if (!CanCreateInstanceOnStack())
			{
				Log::Error("Class::CreateInstanceOnStack : the class [%s] cannot be instanciated", name.c_str());
				return false;
			}
			create_instance_on_stack_func([func](Object * object)
			{
				func(auto_cast_checked(object));
			});
			return true;
		}

		/** returns whether the class has been registered */
		bool IsDeclared() const;
		/** gets the class size */
		size_t GetClassSize() const { return class_size; }
		/** gets the parent class */
		Class const* GetParentClass() const { return parent; }
		/** gets the class name */
		std::string const& GetClassName() const { return name; }
		/** gets the short name */
		std::string const& GetShortName() const { return short_name; }
		/** returns whether we can create instances */
		bool CanCreateInstance() const { return create_instance_func != nullptr; }
		/** returns whether we can create instances */
		bool CanCreateInstanceOnStack() const { return create_instance_on_stack_func != nullptr; }

		/** static inheritance method */
		static InheritanceType InheritsFrom(Class const* child_class, Class const* parent_class, bool accept_equal = false);
		/** returns whether the class inherits from parent */
		InheritanceType InheritsFrom(Class const* parent_class, bool accept_equal = false) const;

	protected:

		/** set the short name */
		void SetShortName(std::string in_short_name);

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
		std::function<Object* ()> create_instance_func;
		/** delegate to create a temporary instance of the object on the stack and call a functor on it */
		std::function<void(std::function<void(Object*)>)> create_instance_on_stack_func;
		/** additionnal initialization for JSONSerializable objects */
		nlohmann::json json_data;
		/** the type_info for the class */
		std::type_info const* info = nullptr;
	};

#endif

}; // namespace chaos