#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	template<typename T>
	class SubClassOf;

}; // namespace chaos

#else 

namespace chaos
{

	/**
	 * SubClassOf : store a reference on a class and use it for object instanciation with type safety
	 */

	template<typename T>
	class SubClassOf
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
					LogTools::Error("SubClassOf constructor : FindClass<T> failure");
				else if (src->InheritsFrom(base_class, true) != InheritanceType::YES)
					LogTools::Error("SubClassOf constructor : src class does not inherit from base_class");
				else
					internal_class = src;
			}
		}
		/** constructor */
		template<typename U>
		SubClassOf(SubClassOf<U> const& src) : SubClassOf(src.GetInternalClass())
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

		/** assign operator */
		SubClassOf<T>& operator = (Class const* src)
		{
			*this = SubClassOf<T>(src);
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
	bool SaveIntoJSON(nlohmann::json& json_entry, SubClassOf<T> const& src)
	{
		Class const* cls = src.GetInternalClass();

		json_entry = (cls == nullptr || !cls->IsDeclared()) ? "" : cls->GetClassName().c_str();
		return true;
	}

	template<typename T>
	bool LoadFromJSON(nlohmann::json const& json_entry, SubClassOf<T>& src)
	{
		std::string classname;
		if (!LoadFromJSON(json_entry, classname))
			return false;
		src = SubClassOf<T>(Class::FindClass(classname.c_str()));
		return true;
	}

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
