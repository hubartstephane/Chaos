namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class SubClassOf;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * SubClassOf : store a reference on a class and use it for object instanciation with type safety
	 */

	template<typename T>
	class SubClassOf
	{
	public:

		/** constructor */
		SubClassOf():
			SubClassOf(T::GetStaticClass())
		{
		}
		/** constructor */
		SubClassOf(SubClassOf const& src) = default;
		/** constructor */
		SubClassOf(SubClassOf && src) = default;
		/** constructor */
		SubClassOf(Class const* src)
		{
			if (src != nullptr)
			{
				Class const* base_class = ClassManager::GetDefaultInstance()->FindCPPClass<T>();
				if (base_class == nullptr)
					Log::Error("SubClassOf constructor : FindCPPClass<T> failure");
				else if (src->InheritsFrom(base_class, true) != InheritanceType::YES)
					Log::Error("SubClassOf constructor : src class does not inherit from base_class");
				else
					internal_class = src;
			}
		}
		/** constructor */
		template<typename U>
		requires std::is_base_of_v<T, U>
		SubClassOf(SubClassOf<U> const & src) :
			SubClassOf(src.GetInternalClass())
		{
		}
		/** constructor */
		template<typename U>
		requires std::is_base_of_v<T, U>
		SubClassOf(SubClassOf<U> && src):
			SubClassOf(src.GetInternalClass())
		{
		}
		/** constructor with search */
		SubClassOf(ClassFindResult find_result):
			SubClassOf(find_result.Resolve(ClassManager::GetDefaultInstance()->FindCPPClass<T>()))
		{
		}
		/** constructor with search */
		SubClassOf(char const* name, ClassManager * manager = ClassManager::GetDefaultInstance()):
			SubClassOf(manager->FindClass(name))
		{
		}

		/** assign operator */
		SubClassOf<T>& operator = (Class const* src)
		{
			*this = SubClassOf<T>(src);
			return *this;
		}
		/** assign operator */
		SubClassOf<T>& operator = (ClassFindResult find_result)
		{
			*this = SubClassOf<T>(find_result);
			return *this;
		}
		/** assign operator */
		SubClassOf<T>& operator = (SubClassOf<T> const& src)
		{
			internal_class = src.internal_class;
			return *this;
		}
		/** assign operator */
		SubClassOf<T>& operator = (SubClassOf<T> && src)
		{
			internal_class = src.internal_class;
			return *this;
		}
		/** assign operator */
		template<typename U>
		requires std::is_base_of_v<T, U>
		SubClassOf<T>& operator = (SubClassOf<U> const & src)
		{
			*this = SubClassOf<T>(src);
			return *this;
		}
		/** assign operator */
		template<typename U>
		requires std::is_base_of_v<T, U>
		SubClassOf<T>& operator = (SubClassOf<U>&& src)
		{
			*this = SubClassOf<T>(src);
			return *this;
		}

		/** validity operator */
		operator bool() const { return IsValid(); }
		/** returns whether the object is valid */
		bool IsValid() const { return (internal_class != nullptr); }

		/** method to create an instance of the object */
		T* CreateInstance() const
		{
			if (internal_class == nullptr)
				return nullptr;
			return (T*)internal_class->CreateInstance();
		}

		/** Subclass may be used as a function to create an instance */
		T* operator () () const
		{
			return CreateInstance();
		}

		/** create a temporary instance on the stack an call the functor on it */
		template<typename FUNC>
		bool CreateInstanceOnStack(FUNC const & func) const
		{
			if (internal_class == nullptr)
				return false;
			return internal_class->CreateInstanceOnStack(func);
		}
		/** get the internal class */
		Class const* GetInternalClass() const { return internal_class; }

	protected:

		/** the internal class used for that */
		Class const* internal_class = nullptr;
	};

	template<typename T>
	std::vector<SubClassOf<T>> GetSubClassesFromString(char const* src, char separator = ',')
	{
		std::vector<SubClassOf<T>> result;

		std::vector<std::string> class_names = StringTools::Split(src, separator);
		for (auto& class_name : class_names)
		{
			class_name.erase(std::remove_if(class_name.begin(), class_name.end(), isspace), class_name.end());
			SubClassOf<T> cls = ClassManager::GetDefaultInstance()->FindClass(class_name.c_str());
			if (cls.IsValid())
				result.push_back(cls);
		}
		return result;
	}


	template<typename T>
	bool DoSaveIntoJSON(nlohmann::json * json, SubClassOf<T> const& src)
	{
		Class const* cls = src.GetInternalClass();

		json = (cls == nullptr || !cls->IsDeclared()) ? "" : cls->GetClassName().c_str();
		return true;
	}

	template<typename T>
	bool DoLoadFromJSON(JSONReadConfiguration config, SubClassOf<T>& src)
	{
		std::string classname;
		if (!LoadFromJSON(config, classname))
			return false;
		src = SubClassOf<T>(ClassManager::GetDefaultInstance()->FindClass(classname.c_str()));
		return src.IsValid();
	}

#endif

}; // namespace chaos