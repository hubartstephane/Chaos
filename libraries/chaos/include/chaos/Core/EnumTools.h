namespace chaos
{
	namespace EnumTools
	{
#ifdef CHAOS_FORWARD_DECLARATION

		template<typename T>
		class EnumMetaDataEntry;

		template<typename T>
		class EnumMetaData;

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_DECLARE_ENUM_FLAG_METHOD(enum_type, ...)\
__VA_ARGS__ enum_type operator|(enum_type a, enum_type b);\
__VA_ARGS__ enum_type operator&(enum_type a, enum_type b);\
__VA_ARGS__ enum_type operator~(enum_type a);\
__VA_ARGS__ enum_type & operator|=(enum_type & a, enum_type b);\
__VA_ARGS__ enum_type & operator&=(enum_type & a, enum_type b);

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_IMPLEMENT_ENUM_FLAG_METHOD(enum_type, ...)\
__VA_ARGS__ enum_type operator|(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<int>(a) | static_cast<int>(b));}\
__VA_ARGS__ enum_type operator&(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<int>(a) & static_cast<int>(b));};\
__VA_ARGS__ enum_type operator~(enum_type a){ return static_cast<enum_type>(~static_cast<int>(a));}\
__VA_ARGS__ enum_type& operator|=(enum_type& a, enum_type b) { a = a | b; return a; }\
__VA_ARGS__ enum_type& operator&=(enum_type& a, enum_type b) { a = a & b; return a; }

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_DECLARE_ENUM_METHOD(enum_type, ...)\
__VA_ARGS__ bool StringToEnum(char const * src, enum_type& dst);\
__VA_ARGS__ char const * EnumToString(enum_type src);\
__VA_ARGS__ chaos::EnumTools::EnumMetaData<enum_type> const & GetEnumMetaData(boost::mpl::identity<enum_type>);

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_IMPLEMENT_ENUM_METHOD(enum_type, metadata, ...)\
__VA_ARGS__ bool StringToEnum(char const * src, enum_type& dst)\
{\
	return metadata.StringToValue(src, dst);\
}\
__VA_ARGS__ char const * EnumToString(enum_type src)\
{\
	return metadata.ValueToString(src);\
}\
__VA_ARGS__ chaos::EnumTools::EnumMetaData<enum_type> const& GetEnumMetaData(boost::mpl::identity<enum_type>)\
{\
	return metadata;\
}

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * EnumMetaDataEntry: used to define a relation between a name and an enum value
	 */
	template<typename T>
	class EnumMetaDataEntry
	{
	public:

		/** the value for this entry */
		T value;
		/** the name for this entry */
		char const* name = nullptr;
	};

	/**
	 * EnumMetaData: some kind of table to make relation between enum names and values
	 */

	template<typename T>
	class EnumMetaData
	{
	public:

		/** constructor */
		EnumMetaData(std::initializer_list<EnumMetaDataEntry<T>> entries)
		{
			// transform a list of entries into 2 list of names/values
			values.reserve(entries.size());
			names.reserve(entries.size());

			for (EnumMetaDataEntry<T> const& entry : entries)
			{
				values.push_back(entry.value);
				names.push_back(entry.name);
			}
		}

		/** gets the number of entry */
		size_t GetCount() const
		{
			return values.size();
		}

		/** get the index for a name */
		std::optional<size_t> GetNameIndex(char const* src) const
		{
			for (size_t i = 0; i < names.size(); ++i)
				if (chaos::StringTools::Stricmp(src, names[i]) == 0)
					return i;
			return {};
		}

		/** get the index for a value */
		std::optional<size_t> GetValueIndex(T src) const
		{
			for (size_t i = 0; i < values.size(); ++i)
				if (src == values[i])
					return i;
			return {};
		}

		/** get a name by index */
		char const* GetNameByIndex(size_t index) const
		{
			assert(index < names.size());
			return names[index];
		}

		/** get a value by index */
		T GetValueByIndex(size_t index) const
		{
			assert(index < values.size());
			return values[index];
		}

		/** convert string into value */
		bool StringToValue(char const* src, T & dst) const
		{
			if (std::optional<size_t> index = GetNameIndex(src))
			{
				dst = GetValueByIndex(index.value());
				return true;
			}
			return false;
		}

		/** convert value into string */
		char const * ValueToString(T value) const
		{
			if (std::optional<size_t> index = GetValueIndex(value))
				return GetNameByIndex(index.value());
			return nullptr;
		}

		/** check whether there is at least one entries */
		bool IsValid() const
		{
			return
				(names.size() == values.size()) &&
				(names.size() > 0);
		}

	public:

		std::vector<T> values;
		std::vector<char const*> names;
	};

#endif

	}; // namespace EnumTools

}; // namespace chaos
