namespace chaos
{

	/**
	* StringTools : a namespace for string related functions
	*/

	namespace StringTools
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** transform any object that have stream operator into a string */
		template<typename T>
		/*CHAOS_API*/ std::string ToString(T const & value)
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}

        /** get the numerical suffix of the input string (ex. toto_123 => 123) */
		CHAOS_API size_t GetStringNumSuffix(std::string_view s);

		/** skip numerics characters (or not) */
		CHAOS_API char const * SkipNumber(char const * c, bool reverse_condition = false);
		/** skip non numerics characters and call atoi */
		CHAOS_API int SkipAndAtoi(char const * c);
		/** returns true whether the character is valid for a variable name */
		CHAOS_API bool IsVariableCharacter(char c);
		/** returns true whether the name is a standard variable name (strict indicates that name cannot start with a figure) */
		CHAOS_API bool IsVariableName(char const * name, bool strict);
		/** create a string from a format */
		CHAOS_API std::string Printf(char const * format, ...);
		/** create a string from a format */
		CHAOS_API std::string VPrintf(char const * format, va_list va);
		/** remove end line */
		CHAOS_API std::string RemoveEndOfLines(std::string const & src);
		/** returns a spaces made string */
		CHAOS_API std::string IndentString(size_t count);

		/** returns a string representation of now  */
		CHAOS_API std::string TimeToString(bool full_string);
		/** returns a string representation of a date */
		CHAOS_API std::string TimeToString(std::time_t t, bool full_string);

		/** skip 0x prefix if existing */
		CHAOS_API char const * SkipHEXPrefix(char const * c);
		/** get the value of a character in hexadeximal meaning (0-15) */
		CHAOS_API unsigned int GetHexValueForChar(char c);
		/** parse a string into hexadeximal value (with or without 0x prefix) */
		CHAOS_API unsigned int AtoiH(char const * str);

		/** trim string to the left and to the right to only keep the base64 characters */
		CHAOS_API std::string TrimBase64(char const * src);
		/** trim string to the left and to the right */
		CHAOS_API std::string TrimSpaces(char const * src, bool trim_left, bool trim_right);

		/** split the input string in comma separated values */
		CHAOS_API std::vector<std::string> Split(char const * src, char delimiter);

		/** some comparaison short cuts */
		CHAOS_API int Strcmp(char const * src1, char const * src2);
		/** some comparaison short cuts */
		CHAOS_API int Strcmp(char const * src1, std::string const & src2);
		/** some comparaison short cuts */
		CHAOS_API int Strcmp(std::string const & src1, char const * src2);
		/** some comparaison short cuts */
		CHAOS_API int Strcmp(std::string const & src1, std::string const & src2);
		/** some comparaison short cuts */
		CHAOS_API int Stricmp(char const * src1, char const * src2);
		/** some comparaison short cuts */
		CHAOS_API int Stricmp(char const * src1, std::string const & src2);
		/** some comparaison short cuts */
		CHAOS_API int Stricmp(std::string const & src1, char const * src2);
		/** some comparaison short cuts */
		CHAOS_API int Stricmp(std::string const & src1, std::string const & src2);

		/** some comparaison short cuts */
		CHAOS_API int Strncmp(char const * src1, char const * src2, size_t count);
		/** some comparaison short cuts */
		CHAOS_API int Strncmp(char const * src1, std::string const & src2, size_t count);
		/** some comparaison short cuts */
		CHAOS_API int Strncmp(std::string const & src1, char const * src2, size_t count);
		/** some comparaison short cuts */
		CHAOS_API int Strncmp(std::string const & src1, std::string const & src2, size_t count);
		/** some comparaison short cuts */
		CHAOS_API int Strnicmp(char const * src1, char const * src2, size_t count);
		/** some comparaison short cuts */
		CHAOS_API int Strnicmp(char const * src1, std::string const & src2, size_t count);
		/** some comparaison short cuts */
		CHAOS_API int Strnicmp(std::string const & src1, char const * src2, size_t count);
		/** some comparaison short cuts */
		CHAOS_API int Strnicmp(std::string const & src1, std::string const & src2, size_t count);

		/** returns true whether the string is null or as 0 length */
		CHAOS_API bool IsEmpty(char const * src);
		/** returns true whether the string is null or as 0 length */
		CHAOS_API bool IsEmpty(std::string const & src);

		/** string comparator class base */
		template<typename COMPARE, int (&Func)(char const *, char const *)>
		class CHAOS_API RawStringCompareBase
		{
		public:

			bool operator ()(char const* src1, char const* src2) const
			{
				return comparator(Func(src1, src2), 0);
			}

			bool operator ()(char const* src1, std::string const & src2) const
			{
				return operator()(src1, src2.c_str());
			}

			bool operator ()(std::string const & src1, char const * src2) const
			{
				return operator()(src1.c_str(), src2);
			}

			bool operator ()(std::string const & src1, std::string const & src2) const
			{
				return operator()(src1.c_str(), src2.c_str());
			}

		protected:

			COMPARE comparator;
		};

		using RawStringLess = RawStringCompareBase<std::less<int>, &StringTools::Strcmp>;
		using RawStringGreater = RawStringCompareBase<std::greater<int>, &StringTools::Strcmp>;
		using RawStringEqualTo = RawStringCompareBase<std::equal_to<int>, &StringTools::Strcmp>;

		using RawStringILess = RawStringCompareBase<std::less<int>, &StringTools::Stricmp>;
		using RawStringIGreater = RawStringCompareBase<std::greater<int>, &StringTools::Stricmp>;
		using RawStringIEqualTo = RawStringCompareBase<std::equal_to<int>, &StringTools::Stricmp>;

#endif

	}; // namespace StringTools

}; // namespace chaos




