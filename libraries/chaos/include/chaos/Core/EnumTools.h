
#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace EnumTools
	{
		template<typename T>
		class EnumMetaDataEntry;

		template<typename T>
		class EnumMetaData;
	
	}; // namespace EnumTools

	template<typename T>
	concept IsEnumBitmask = requires()
	{
		{IsEnumBitmaskImpl(boost::mpl::identity<typename std::remove_const<T>::type>())};
	};

}; // namespace chaos

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_DECLARE_ENUM_BITMASK_METHOD(enum_type, ...)\
__VA_ARGS__ enum_type operator|(enum_type a, enum_type b);\
__VA_ARGS__ enum_type operator&(enum_type a, enum_type b);\
__VA_ARGS__ enum_type operator~(enum_type a);\
__VA_ARGS__ enum_type & operator|=(enum_type & a, enum_type b);\
__VA_ARGS__ enum_type & operator&=(enum_type & a, enum_type b);\
__VA_ARGS__ bool IsEnumBitmaskImpl(boost::mpl::identity<enum_type>);\
__VA_ARGS__ bool IsNull(enum_type src);\
__VA_ARGS__ bool HasAnyFlags(enum_type src, enum_type flags);\
__VA_ARGS__ bool HasAllFlags(enum_type src, enum_type flags);

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(enum_type, ...)\
__VA_ARGS__ enum_type operator|(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<int>(a) | static_cast<int>(b));}\
__VA_ARGS__ enum_type operator&(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<int>(a) & static_cast<int>(b));};\
__VA_ARGS__ enum_type operator~(enum_type a){ return static_cast<enum_type>(~static_cast<int>(a));}\
__VA_ARGS__ enum_type& operator|=(enum_type& a, enum_type b) { a = a | b; return a; }\
__VA_ARGS__ enum_type& operator&=(enum_type& a, enum_type b) { a = a & b; return a; }\
__VA_ARGS__ bool IsEnumBitmaskImpl(boost::mpl::identity<enum_type>){ return true;}\
__VA_ARGS__ bool IsNull(enum_type src)\
{\
	return static_cast<int>(src) == 0;\
}\
__VA_ARGS__ bool HasAnyFlags(enum_type src, enum_type flags)\
{\
	assert(static_cast<int>(flags) != 0);\
	return (static_cast<int>(src) & static_cast<int>(flags)) != 0;\
}\
__VA_ARGS__ bool HasAllFlags(enum_type src, enum_type flags)\
{\
	assert(static_cast<int>(flags) != 0);\
	return (static_cast<int>(src) & static_cast<int>(flags)) == static_cast<int>(flags);\
}


/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_DECLARE_ENUM_METHOD(enum_type, ...)\
__VA_ARGS__ bool StringToEnum(char const * src, enum_type& dst);\
__VA_ARGS__ char const * EnumToString(enum_type src, char * buffer = nullptr, size_t buflen = 0);\
__VA_ARGS__ chaos::EnumTools::EnumMetaData<enum_type> const & GetEnumMetaData(boost::mpl::identity<enum_type>);\
__VA_ARGS__ std::istream & operator >> (std::istream& stream, enum_type& dst);\
__VA_ARGS__ std::ostream & operator << (std::ostream& stream, enum_type src);

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_IMPLEMENT_ENUM_METHOD(enum_type, metadata, ...)\
__VA_ARGS__ bool StringToEnum(char const * src, enum_type& dst)\
{\
	return metadata.StringToValue(src, dst);\
}\
__VA_ARGS__ char const * EnumToString(enum_type src, char * buffer, size_t buflen)\
{\
	return metadata.ValueToString(src, buffer, buflen);\
}\
__VA_ARGS__ chaos::EnumTools::EnumMetaData<enum_type> const& GetEnumMetaData(boost::mpl::identity<enum_type>)\
{\
	return metadata;\
}\
__VA_ARGS__ std::istream& operator >> (std::istream& stream, enum_type& dst)\
{\
	std::string tmp;\
	stream >> tmp;\
	StringToEnum(tmp.c_str(), dst);\
	return stream;\
}\
__VA_ARGS__ std::ostream& operator << (std::ostream& stream, enum_type src)\
{\
	stream << EnumToString(src);\
	return stream;\
}

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * IsEnumBitmask: whether the enum is to be considered as a bit mask
	 */

namespace chaos
{
	namespace EnumTools
	{
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
			char const * ValueToString(T value, char * buffer = nullptr, size_t buflen = 0) const
			{
				int value_as_int = static_cast<int>(value);

				if (!IsEnumBitmask<T> || (value_as_int == 0) || (MathTools::IsPowerOf2(value_as_int))) // a single or no bit. Normal search
				{
					if (std::optional<size_t> index = GetValueIndex(value))
						return GetNameByIndex(index.value());
				}
				else
				{
					assert(buffer != nullptr);
					assert(buflen > 0);

					std::ostrstream stream(buffer, buflen);

					BitTools::ForEachBitForward(value_as_int, [this, &stream](int index)
					{
						T bit_as_value = static_cast<T>(1 << index);

						if (std::optional<size_t> index = GetValueIndex(bit_as_value))
						{
							if (stream.pcount() > 0)
								stream << "|";
							stream << GetNameByIndex(index.value());
						}
					});

					buffer[stream.pcount()] = 0; // terminal zero

					return buffer;

				}
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

	}; // namespace EnumTools

}; // namespace chaos

#endif