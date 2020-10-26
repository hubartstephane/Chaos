#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace EnumTools
	{

	}; // namespace EnumTools

}; // namespace chaos

#else 

namespace chaos
{
	namespace EnumTools
	{
		/** decode a value with a conversion table */
		template<typename T, typename ENCODE_TABLE>
		bool StringToEnum(char const * str, ENCODE_TABLE const & encode_table, T & result)
		{
			for (auto const & encode : encode_table)
			{
				if (StringTools::Stricmp(str, encode.second) == 0)
				{
					result = encode.first;
					return true;
				}
			}
			return false;
		}

		/** encode a value with a conversion table */
		template<typename T, typename ENCODE_TABLE>
		bool EnumToString(T const & src, ENCODE_TABLE const & encode_table, std::string & result)
		{
			for (auto const & encode : encode_table)
			{
				if (src == encode.first)
				{
					result = encode.second;
					return true;
				}
			}
			return false;
		}
	}; // namespace EnumTools

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
