namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	enum class TimeToStringFormatType;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * TimeToStringFormatType: How to format a date
	 **/

	enum class CHAOS_API TimeToStringFormatType : int
	{
		SHORT,
		FULL,
		FILENAME
	};

	/**
	 * StringTools : a namespace for string related functions
	 */

	namespace StringTools
	{

		/** transform any object that have stream operator into a string */
		template<typename T>
		std::string ToString(T const & value)
		{
			std::stringstream stream;
			stream << value;
			return stream.str();
		}

		/** split a string according to a separator and call a deleguate on each fragment */
		template<typename FUNC>
		decltype(auto) WithSplittedText(std::string_view src, std::string_view delim, FUNC const& func)
		{
			using L = meta::LambdaInfo<FUNC, char const*>;

			char tmp_buffer[256];
			std::string tmp_string;

			for (const auto word : std::views::split(src, delim))
			{
				char const* ptr = nullptr;

				// optimized code with a on the stack buffer
				if (word.end() - word.begin() < sizeof(tmp_buffer) - 1)
				{
					// copy view into buffer, add zero at the end
					auto last = std::copy(word.begin(), word.end(), tmp_buffer);
					*last = 0;
					ptr = tmp_buffer;
				}
				// degraded code requiring memory allocation
				else
				{
					tmp_string = std::string(word.begin(), word.end());
					ptr = tmp_string.c_str();
				}

				// call deleguate
				if constexpr (L::convertible_to_bool)
				{
					if (decltype(auto) result = func(ptr))
						return result;
				}
				else
				{
					func(ptr);
				}
			}

			// default result
			if constexpr (L::convertible_to_bool)
				return typename L::result_type{};
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

		/** returns a string representation of a date */
		CHAOS_API std::string TimeToString(std::time_t t, TimeToStringFormatType format);
		/** returns a string representation of a date */
		CHAOS_API std::string TimeToString(std::chrono::system_clock::time_point time_point, TimeToStringFormatType format);

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
		class RawStringCompareBase
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

	}; // namespace StringTools

#endif

}; // namespace chaos




