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
	class CHAOS_API SubClassOf
	{
	public:

		/** constructor */
		SubClassOf() : SubClassOf(T::GetStaticClass())
		{
		}
		/** constructor */
		SubClassOf(SubClassOf const& src) = default;
		/** constructor */
		SubClassOf(Class const* src)
		{
			if (src != nullptr)
			{
				Class const* base_class = Class::FindClass<T>();
				if (base_class == nullptr)
					Log::Error("SubClassOf constructor : FindClass<T> failure");
				else if (src->InheritsFrom(base_class, true) != InheritanceType::YES)
					Log::Error("SubClassOf constructor : src class does not inherit from base_class");
				else
					internal_class = src;
			}
		}
		/** constructor */
		template<typename U>
		SubClassOf(SubClassOf<U> const& src) : SubClassOf(src.GetInternalClass())
		{
		}
		/** constructor with search */
		SubClassOf(ClassFindResult find_result) : SubClassOf(find_result.Resolve(Class::FindClass<T>()))
		{
		}
		/** constructor with search */
		SubClassOf(char const* name) : SubClassOf(Class::FindClass(name))
		{
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
		/** create a temporary instance on the stack an call the functor on it */
		template<typename FUNC>
		bool CreateInstanceOnStack(FUNC func) const
		{
			if (internal_class == nullptr)
				return false;
			return internal_class->CreateInstanceOnStack(func);
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
		template<typename U>
		SubClassOf<T>& operator = (SubClassOf<U> const& src)
		{
			*this = SubClassOf<T>(src);
			return *this;
		}

		/** get the internal class */
		Class const* GetInternalClass() const { return internal_class; }

	protected:

		/** the internal class used for that */
		Class const* internal_class = nullptr;
	};

	template<typename T>
	CHAOS_API std::vector<SubClassOf<T>> GetSubClassesFromString(char const* src, char separator = ',')
	{
		std::vector<SubClassOf<T>> result;

		std::vector<std::string> class_names = StringTools::Split(src, separator);
		for (auto& class_name : class_names)
		{
			class_name.erase(std::remove_if(class_name.begin(), class_name.end(), isspace), class_name.end());
			SubClassOf<T> cls = Class::FindClass(class_name.c_str());
			if (cls.IsValid())
				result.push_back(cls);
		}
		return result;
	}


	template<typename T>
	CHAOS_API bool SaveIntoJSON(nlohmann::json& json, SubClassOf<T> const& src)
	{
		Class const* cls = src.GetInternalClass();

		json = (cls == nullptr || !cls->IsDeclared()) ? "" : cls->GetClassName().c_str();
		return true;
	}

	template<typename T>
	CHAOS_API bool LoadFromJSON(nlohmann::json const& json, SubClassOf<T>& src)
	{
		std::string classname;
		if (!LoadFromJSON(json, classname))
			return false;
		src = SubClassOf<T>(Class::FindClass(classname.c_str()));
		return true;
	}

#endif

}; // namespace chaos