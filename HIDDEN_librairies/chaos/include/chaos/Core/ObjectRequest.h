namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ObjectRequestType;

	class ObjectRequest;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** a utility class for parameter passing */
	enum class CHAOS_API ObjectRequestType
	{
		/** accept nothing */
		NONE = 0,
		/** accept anything */
		ANY = 1,
		/** search by string */
		STRING = 2,
		/** search by tag */
		TAG = 3
	};

	/** a utility class for parameter passing */
	class CHAOS_API ObjectRequest
	{
	public:

		/** constructor */
		ObjectRequest() = default;
		/** constructor */
		ObjectRequest(ObjectRequest const& src) = default;
		/** constructor */
		ObjectRequest(char const* in_name) :
			name(in_name), request_type(ObjectRequestType::STRING)
		{
			if (StringTools::IsEmpty(name))
				request_type = ObjectRequestType::NONE;
		}
		/** constructor */
		ObjectRequest(std::string const& in_name) :
			name(in_name.c_str()), request_type(ObjectRequestType::STRING)
		{
			if (StringTools::IsEmpty(name))
				request_type = ObjectRequestType::NONE;
		}
		/** constructor */
		ObjectRequest(TagType in_tag) :
			tag(in_tag), request_type(ObjectRequestType::TAG)
		{
		}

		/** returns whether the request is none */
		bool IsNoneRequest() const
		{
			return (request_type == ObjectRequestType::NONE);
		}

		/** returns whether the request is any */
		bool IsAnyRequest() const
		{
			return (request_type == ObjectRequestType::ANY);
		}

		/** returns whether this is a request for the string */
		bool IsStringRequest() const
		{
			return (request_type == ObjectRequestType::STRING);
		}

		/** returns whether there is a request for the tag */
		bool IsTagRequest() const
		{
			return (request_type == ObjectRequestType::TAG);
		}

		/** returns a request that accept anything */
		static ObjectRequest Any()
		{
			ObjectRequest result;
			result.request_type = ObjectRequestType::ANY;
			return result;
		}

		/** returns a request that accept nothing */
		static ObjectRequest None()
		{
			ObjectRequest result;
			result.request_type = ObjectRequestType::NONE;
			return result;
		}

		/** test whether the object name/tag match */
		template<typename T>
		bool Match(T const& object) const // use template to use NamedInterfaceWrapper as well as NamedInterface
		{
			if (IsNoneRequest())
				return false;
			if (IsAnyRequest())
				return true;
			if (IsStringRequest())
				return (StringTools::Stricmp(object.GetName(), name) == 0);
			if (IsTagRequest())
				return (object.GetTag() == tag);
			return false;
		}

		/** search element in a vector */
		template<typename CHECK_CLASS = EmptyClass, typename P>
		auto FindObject(std::vector<P>& elements) const -> decltype(meta::get_raw_pointer(elements[0]))
		{
			std::optional<size_t> index = FindObjectIndex<CHECK_CLASS>(elements);
			if (index.has_value())
				return meta::get_raw_pointer(elements[*index]);
			return nullptr;
		}
		/** search element in a vector */
		template<typename CHECK_CLASS = EmptyClass, typename P>
		auto FindObject(std::vector<P> const& elements) const -> decltype(meta::get_raw_pointer(elements[0]))
		{
			std::optional<size_t> index = FindObjectIndex<CHECK_CLASS>(elements);
			if (index.has_value())
				return meta::get_raw_pointer(elements[*index]);
			return nullptr;
		}

		/** search element in a vector */
		template<typename CHECK_CLASS = EmptyClass, typename P>
		std::optional<size_t> FindObjectIndex(std::vector<P>& elements) const
		{
			// early exit
			if (IsNoneRequest())
				return {};
			// search in the list
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
			{
				auto e = meta::get_raw_pointer(elements[i]);
				if (Match(*e))
					if (CheckClass<CHECK_CLASS>(e))
						return i;
			}
			return {};
		}
		/** search element in a vector */
		template<typename CHECK_CLASS = EmptyClass, typename P>
		std::optional<size_t> FindObjectIndex(std::vector<P> const& elements) const
		{
			// early exit
			if (IsNoneRequest())
				return {};
			// search in the list
			size_t count = elements.size();
			for (size_t i = 0; i < count; ++i)
			{
				auto e = meta::get_raw_pointer(elements[i]);
				if (Match(*e))
					if (CheckClass<CHECK_CLASS>(e))
						return i;
			}
			return {};
		}

		/** check whether the element match the wanted class */
		template<typename CHECK_CLASS = EmptyClass, typename T>
		static bool CheckClass(T const* element)
		{
			if constexpr (!std::is_same_v<CHECK_CLASS, EmptyClass>)
				if (dynamic_cast<CHECK_CLASS const*>(element) == nullptr)
					return false;
			return true;
		}

	public:

		/** the name for the request */
		char const* name = nullptr;
		/** the tag for the request */
		TagType tag = 0;
		/** the kind of request of interrest */
		ObjectRequestType request_type = ObjectRequestType::NONE;
	};

#endif

}; // namespace chaos