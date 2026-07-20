namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InheritanceType;

	class ClassBase;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * InheritanceType: the kind if inheritance that can exist between 2 classes
	 */

	enum class InheritanceType : int
	{
		Unknown = -1,
		No = 0,
		Yes = 1
	};

	/**
	 * ClassBase: common base class for Class
	 */

	class CHAOS_API ClassBase : public ReferenceCountedInterface
	{
		friend class ClassManager;
		friend class NativeClassManager;
		friend class ClassLoader;

		template<typename CPP_TYPE>
		friend class CppClassRegisterResult;

	public:

		/** destructor */
		virtual ~ClassBase() = default;

		/** gets the class name */
		std::string const& GetClassName() const { return name; }
		/** gets the short name */
		std::string const& GetShortName() const { return short_name; }
		/** gets the class size */
		size_t GetClassSize() const { return class_size; }
		/** returns whether the class has been registered */
		bool IsFullyInitialized() const { return info != nullptr; }
		/** gets the parent class */
		ClassBase const* GetParentClass() const { return parent_class; }
		/** gets the class manager */
		ClassManager* GetClassManager() const { return manager; } // no need to have a manager const

		/** change the shortname of the class (just used at construction) */
		void SetShortName(std::string in_shorname);

		/** check whether we inherit from another class */
		bool InheritsFrom(ClassBase const* base, bool accept_same = true) const;
		/** check whether we inherit from another class */
		template<typename CPP_TYPE>
		bool InheritsFrom(bool accept_same = true) const;

	protected:

		/** initialize newly created instance (at this point type of 'instance' is unknown) */
		void InitializeInstance(void* instance) const
		{
			if (parent_class != nullptr)
				parent_class->InitializeInstance(instance);
			if (initialize_instance_func)
				initialize_instance_func(instance);
		}

	protected:

		/** the optional name of the class */
		std::string name;
		/** the optional short name of the class */
		std::string short_name;
		/** get class size */
		size_t class_size = 0;
		/** the type_info for the class */
		std::type_info const* info = nullptr;
		/** the parent of the class */
		ClassBase const* parent_class = nullptr;
		/** the manager for this class */
		ClassManager* manager = nullptr;
		/** function to initialize newly created instance */
		std::function<void(void*)> initialize_instance_func;
	};

#endif

}; // namespace chaos