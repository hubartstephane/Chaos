namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class InheritanceType;

	class CHAOS_API ClassBase;

	template<typename CLASS_TYPE>
	class Class;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	CHAOS_DEFINE_LOG(ClassLog, "Class")

	/**
	 * CHAOS_REGISTER_CLASS : a macro that helps register classes automatically
	 */

#define CHAOS_REGISTER_CLASS(CLASS, ...) static inline chaos::Class<CLASS> const * CLASS##_class = chaos::ClassManager::GetDefaultInstance()->DeclareCPPClass<CLASS __VA_OPT__(,) __VA_ARGS__>(#CLASS)

	/**
	 * InheritanceType : the kind if inheritance that can exist between 2 classes
	 */

	enum class InheritanceType : int
	{
		Unknown = -1,
		No = 0,
		Yes = 1
	};

	/**
	 * ClassBase : a registered class
	 */

	class CHAOS_API ClassBase
	{
	protected:

		/** constructor */
		ClassBase(std::string in_name);

	public:

		/** destructor */
		virtual ~ClassBase() = default;

		/** gets the class size */
		size_t GetClassSize() const { return class_size; }
		/** gets the parent class */
		ClassBase const* GetParentClass() const { return parent; }
		/** gets the class name */
		std::string const& GetClassName() const { return name; }
		/** gets the short name */
		std::string const& GetShortName() const { return short_name; }
		/** gets whether the class is already properly declared */
		bool IsDeclared() const { return declared; }

		/** gets the class manager */
		ClassManager* GetClassManager() const { return manager; } // no need to have a manager const

		/** set the parent class */
		void SetParentClass(ClassBase const* in_parent);
		/** set the short name */
		void SetShortName(std::string in_short_name);

		/** static inheritance method */
		static InheritanceType InheritsFrom(ClassBase const* child_class, ClassBase const* parent_class, bool accept_equal = false);
		/** returns whether the class inherits from parent */
		InheritanceType InheritsFrom(ClassBase const* parent_class, bool accept_equal = false) const;

	protected:

#if _DEBUG
		/** search whether the some parent class is in a child manager */
		bool HasCyclicParent() const;
#endif // #if _DEBUG

	protected:

		/** the parent of the class */
		ClassBase const* parent = nullptr;
		/** get class size */
		size_t class_size = 0;
		/** the optional name of the class */
		std::string name;
		/** the optional short name of the class */
		std::string short_name;
		/** whether the class has been fully declared */
		bool declared = false;
		/** the type_info for the class */
		std::type_info const* info = nullptr;
		/** the manager for this class */
		ClassManager* manager = nullptr;
	};

	/**
	 * ClassParentClass: get the parent class of a class
	 */

	template<typename CLASS_TYPE>
	requires
	(
		HasInternalSuperClass<CLASS_TYPE> || HasExternalSuperType<CLASS_TYPE>
	)
	using ClassParentClass = SuperClass_t<CLASS_TYPE>;

	template<typename CLASS_TYPE>
	requires 
	(
		!HasInternalSuperClass<CLASS_TYPE> && !HasExternalSuperType<CLASS_TYPE>
	)
	using ClassParentClass = ClassBase;

	/**
	 * Class: a registered class
	 */

	template<typename CLASS_TYPE>
	class Class : public ClassParentClass<CLASS_TYPE>
	{
		friend class ClassLoader;
		friend class ClassRegistration<CLASS_TYPE>;
		friend class ClassFindResult;
		friend class ClassManager;

	protected:

		//using Super::Super;

	public:

		/** method to create an instance of the object */
		virtual CLASS_TYPE * CreateInstance() const
		{
			CLASS_TYPE * result = new CLASS_TYPE;
			if (result != nullptr)
				InitializeClassInstance(result);
			return result;
		}

		/** creation operator */
		CLASS_TYPE * operator ()() const
		{
			return CreateInstance();
		}

	protected:

		/** initialization entry point */
		void InitializeClassInstance(CLASS_TYPE * instance) const
		{
			// recursive: parents first
			if (parent != nullptr)
				parent->InitializeObjectInstance(instance);
			// apply initialization functors
			OnInstanceInitialization(instance);
		}

		/** initialization specialization */
		virtual void OnInstanceInitialization(CLASS_TYPE* instance) const {}
	};

#endif

}; // namespace chaos