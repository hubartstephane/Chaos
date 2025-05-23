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

		/** concept for conversion */
		template<typename T>
		concept ConvertibleToStringView = std::convertible_to<T, std::string_view>;

		namespace details
		{
			/** internal method to compute joined string size */
			template<ConvertibleToStringView ...PARAMS>
			size_t GetJoinedSizeHelper(std::string_view first, PARAMS... params)
			{
				if constexpr (sizeof...(PARAMS) == 0)
					return first.size();
				else
					return first.size() + GetJoinedSizeHelper(std::forward<PARAMS>(params)...);
			}

			/** internal method to join strings */
			template<ConvertibleToStringView ...PARAMS>
			void JoinHelper(char* dst, size_t dst_size, bool separator_required, std::string_view separator, std::string_view first, PARAMS... params)
			{
				// add separator if necessary
				if (separator_required)
				{
					strcpy_s(dst, dst_size, separator.data());
					dst += separator.size();
					dst_size -= separator.size();
				}
				// add first token
				strcpy_s(dst, dst_size, first.data());
				dst += first.size();
				dst_size -= first.size();
				// add other tokens
				if constexpr (sizeof...(PARAMS) > 0)
					JoinHelper(dst, dst_size, true, separator, std::forward<PARAMS>(params)...);
			}
		};

		/** compute the size of a joined string */
		template<ConvertibleToStringView ...PARAMS>
		size_t GetJoinedSize(std::string_view separator, PARAMS... params)
		{
			size_t result = details::GetJoinedSizeHelper(std::forward<PARAMS>(params)...);
			if constexpr (sizeof...(PARAMS) > 1)
				result += separator.size() * (sizeof...(PARAMS) - 1);
			return result;
		}

		/** join all parameters into a single string */
		template<ConvertibleToStringView ...PARAMS>
		std::string Join(std::string_view separator, PARAMS... params)
		{
			// early exit
			if constexpr (sizeof...(PARAMS) == 0)
				return {};
			// compute the required size
			size_t size = GetJoinedSize(separator, std::forward<PARAMS>(params)...);
			// allocate a string large enough
			std::string result = std::string(size + 1, 0);
			// appends all required tokens
			details::JoinHelper(&result[0], size + 1, false, separator, std::forward<PARAMS>(params)...);
			return result;
		}

		/** read an integer number as a decimal, a binary or an hexadecimal */
		template<meta::IsInteger T>
		char const* ReadInteger(char const* str, T & dst)
		{
			T result = 0;
			char const* p = str;
			bool one_figure = false;

			auto ConsumSpaces = [&p]() // consum all spaces
			{
				while (*p != 0 && isspace(*p))
					++p;
			};

			auto ReadDecimal = [&result, &p, &one_figure]() // read a decimal (without sign)
			{
				while (*p >= '0' && *p <= '9')
				{
					result = (result * 10) + (*p - '0');
					one_figure = true;
					++p;
				}
			};

			auto CommitReading = [&result, &p, &one_figure, &dst, &str]() // commit the reading if at least one figure has been read
			{
				if (one_figure)
				{
					dst = result;
					return p;
				}
				return str;
			};

			// eat initial separators	
			ConsumSpaces();
			// consume other caracters
			if (*p == 0)
			{
				return str; // no figure read, don't eat any character
			}

			// negative DECIMAL number (don't use a sign for binary or hexadecimal)
			if constexpr (std::is_signed_v<T>)
			{
				if (*p == '-') 
				{
					++p;
					ConsumSpaces();
					ReadDecimal();
					result = -result;
					return CommitReading();
				}
			}

			// positive DECIMAL number (don't use a sign for binary or hexadecimal)
			if (*p == '+')
			{
				++p;
				ConsumSpaces();
				ReadDecimal();
				return CommitReading();
			}

			// could be decimal, hexadecimal, binary
			if (*p == '0')
			{
				++p;
				if (*p == 'b' || *p == 'B') // binary
				{
					++p;
					while (*p >= '0' && *p <= '1')
					{
						result = (result << 1) + (*p - '0');
						one_figure = true;
						++p;
					}
				}
				else if (*p == 'x' || *p == 'X') // hexadecimal
				{
					++p;
					while (*p != 0)
					{
						char value = 0;
						if (*p >= '0' && *p <= '9')
							value = *p - '0';
						else if (*p >= 'a' && *p <= 'f')
							value = 10 + (*p - 'a');
						else if (*p >= 'A' && *p <= 'F')
							value = 10 + (*p - 'A');
						else
							break;
						result = (result * 16) + value;
						one_figure = true;
						++p;
					}
				}
				else
				{
					one_figure = true; // the initial 0 is a valid first character
					ReadDecimal();
				}
			}
			else
			{
				ReadDecimal();
			}

			return CommitReading();
		}

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




