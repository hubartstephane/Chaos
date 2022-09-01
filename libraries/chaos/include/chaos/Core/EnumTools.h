namespace chaos
{
	namespace EnumTools
	{
#if defined CHAOS_FORWARD_DECLARATION

	template<typename T, typename ENCODE_TABLE>
	CHAOS_API bool ConvertStringToEnum(char const* src, ENCODE_TABLE const& encode_table, T& dst);

	template<typename T, typename ENCODE_TABLE>
	CHAOS_API bool ConvertEnumToString(T src, ENCODE_TABLE const& encode_table, std::string& dst);

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
CHAOS_API bool EnumToString(enum_type src, std::string & dst);

#define CHAOS_IMPLEMENT_ENUM_METHOD(enum_type, table_name)\
CHAOS_API bool StringToEnum(char const * src, enum_type& dst)\
{\
	return EnumTools::ConvertStringToEnum(src, table_name, dst);\
}\
CHAOS_API bool EnumToString(enum_type src, std::string & dst)\
{\
	return EnumTools::ConvertEnumToString(src, table_name, dst);\
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
	bool ConvertEnumToString(T src, ENCODE_TABLE const & encode_table, std::string & dst)
	{
		for (auto const & encode : encode_table)
		{
			if (src == encode.first)
			{
				dst = encode.second;
				return true;
			}
		}
		return false;
	}
#endif

	}; // namespace EnumTools

}; // namespace chaos
