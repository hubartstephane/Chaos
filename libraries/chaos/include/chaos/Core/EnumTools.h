namespace chaos
{
	namespace EnumTools
	{
#if defined CHAOS_FORWARD_DECLARATION

	template<typename T>
	using EnumMetaData = std::vector<std::pair<T, char const*>>;

	template<typename T, typename ENCODE_TABLE>
	bool ConvertStringToEnum(char const* src, ENCODE_TABLE const& encode_table, T& dst);

	template<typename T, typename ENCODE_TABLE>
	char const * ConvertEnumToString(T src, ENCODE_TABLE const& encode_table);

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
CHAOS_API char const * EnumToString(enum_type src);

#define CHAOS_IMPLEMENT_ENUM_METHOD(enum_type, table_name)\
CHAOS_API bool StringToEnum(char const * src, enum_type& dst)\
{\
	return chaos::EnumTools::ConvertStringToEnum(src, table_name, dst);\
}\
CHAOS_API char const * EnumToString(enum_type src)\
{\
	return chaos::EnumTools::ConvertEnumToString(src, table_name);\
}

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** decode a value with a conversion table */
	template<typename T, typename ENCODE_TABLE>
	bool ConvertStringToEnum(char const * src, ENCODE_TABLE const & encode_table, T & dst)
	{
		for (auto const & encode : encode_table)
		{
			if (StringTools::Stricmp(src, encode.second) == 0)
			{
				dst = encode.first;
				return true;
			}
		}
		return false;
	}

	/** encode a value with a conversion table */
	template<typename T, typename ENCODE_TABLE>
	char const * ConvertEnumToString(T src, ENCODE_TABLE const & encode_table)
	{
		for (auto const & encode : encode_table)
		{
			if (src == encode.first)
			{
				return encode.second;
			}
		}
		return nullptr;
	}
#endif

	}; // namespace EnumTools

}; // namespace chaos
