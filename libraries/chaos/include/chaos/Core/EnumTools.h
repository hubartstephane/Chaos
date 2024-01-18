namespace chaos
{
	namespace EnumTools
	{
#if defined CHAOS_FORWARD_DECLARATION

	template<typename T>
	using EnumMetaData = std::vector<std::pair<T, char const*>>;

	template<typename T, typename METADATA>
	bool ConvertStringToEnum(char const* src, METADATA const& metadata, T& dst);

	template<typename T, typename METADATA>
	char const * ConvertEnumToString(T src, METADATA const& metadata);

#define CHAOS_DECLARE_ENUM_FLAG_METHOD(enum_type)\
CHAOS_API enum_type operator|(enum_type a, enum_type b);\
CHAOS_API enum_type operator&(enum_type a, enum_type b);\
CHAOS_API enum_type operator~(enum_type a);\
CHAOS_API enum_type & operator|=(enum_type & a, enum_type b);\
CHAOS_API enum_type & operator&=(enum_type & a, enum_type b);

#define CHAOS_IMPLEMENT_ENUM_FLAG_METHOD(enum_type)\
CHAOS_API enum_type operator|(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<int>(a) | static_cast<int>(b));}\
CHAOS_API enum_type operator&(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<int>(a) & static_cast<int>(b));};\
CHAOS_API enum_type operator~(enum_type a){ return static_cast<enum_type>(~static_cast<int>(a));}\
CHAOS_API enum_type& operator|=(enum_type& a, enum_type b) { a = a | b; return a; }\
CHAOS_API enum_type& operator&=(enum_type& a, enum_type b) { a = a & b; return a; }

#define CHAOS_DECLARE_ENUM_METHOD(enum_type)\
CHAOS_API bool StringToEnum(char const * src, enum_type& dst);\
CHAOS_API char const * EnumToString(enum_type src);\
CHAOS_API chaos::EnumTools::EnumMetaData<enum_type> const & GetEnumMetaData(boost::mpl::identity<enum_type>);

#define CHAOS_IMPLEMENT_ENUM_METHOD(enum_type, metadata)\
CHAOS_API bool StringToEnum(char const * src, enum_type& dst)\
{\
	return chaos::EnumTools::ConvertStringToEnum(src, metadata, dst);\
}\
CHAOS_API char const * EnumToString(enum_type src)\
{\
	return chaos::EnumTools::ConvertEnumToString(src, metadata);\
}\
CHAOS_API chaos::EnumTools::EnumMetaData<enum_type> const& GetEnumMetaData(boost::mpl::identity<enum_type>)\
{\
	return metadata;\
}

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** decode a value with a conversion table */
	template<typename T, typename METADATA>
	bool ConvertStringToEnum(char const * src, METADATA const & metadata, T & dst)
	{
		for (auto const & data : metadata)
		{
			if (StringTools::Stricmp(src, data.second) == 0)
			{
				dst = data.first;
				return true;
			}
		}
		return false;
	}

	/** encode a value with a conversion table */
	template<typename T, typename METADATA>
	char const * ConvertEnumToString(T src, METADATA const & metadata)
	{
		for (auto const & data : metadata)
		{
			if (src == data.first)
			{
				return data.second;
			}
		}
		return nullptr;
	}
#endif

	}; // namespace EnumTools

}; // namespace chaos
