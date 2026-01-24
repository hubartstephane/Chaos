
#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	template<typename T>
	class EnumMetaDataEntry;

	template<typename T>
	class EnumMetaData;

	template<typename T>
	class EnumBitmaskMetaData;

	CHAOS_GENERATE_VALUE_MAPPING_DECLARATION(IsEnumBitmask, typename, bool, false);

}; // namespace chaos

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_DECLARE_ENUM_BOOL_METHOD(enum_type, ...)\
__VA_ARGS__ enum_type operator|(enum_type a, enum_type b);\
__VA_ARGS__ enum_type operator&(enum_type a, enum_type b);\
__VA_ARGS__ enum_type operator!(enum_type a);\
__VA_ARGS__ enum_type & operator|=(enum_type & a, enum_type b);\
__VA_ARGS__ enum_type & operator&=(enum_type & a, enum_type b);

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_IMPLEMENT_ENUM_BOOL_METHOD(enum_type, ...)\
__VA_ARGS__ enum_type operator|(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<bool>(a) | static_cast<bool>(b));}\
__VA_ARGS__ enum_type operator&(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<bool>(a) & static_cast<bool>(b));};\
__VA_ARGS__ enum_type operator!(enum_type a){ return static_cast<enum_type>(!static_cast<bool>(a));}\
__VA_ARGS__ enum_type& operator|=(enum_type& a, enum_type b) { a = a | b; return a; }\
__VA_ARGS__ enum_type& operator&=(enum_type& a, enum_type b) { a = a & b; return a; }

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_DECLARE_ENUM_BITMASK_METHOD(enum_type, ...)\
CHAOS_GENERATE_VALUE_MAPPING_SPECIALIZATION(IsEnumBitmask, enum_type, bool, true)\
__VA_ARGS__ enum_type operator|(enum_type a, enum_type b);\
__VA_ARGS__ enum_type operator&(enum_type a, enum_type b);\
__VA_ARGS__ enum_type operator~(enum_type a);\
__VA_ARGS__ enum_type & operator|=(enum_type & a, enum_type b);\
__VA_ARGS__ enum_type & operator&=(enum_type & a, enum_type b);\
__VA_ARGS__ chaos::EnumBitmaskMetaData<enum_type> const * GetEnumBitmaskMetaData(boost::mpl::identity<enum_type>);\
__VA_ARGS__ bool IsNull(enum_type src);\
__VA_ARGS__ bool HasAnyFlags(enum_type src, enum_type flags);\
__VA_ARGS__ bool HasAllFlags(enum_type src, enum_type flags);\
__VA_ARGS__ bool AreValidFlags(enum_type src);

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(enum_type, bitmask_metadata, ...)\
__VA_ARGS__ enum_type operator|(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<int>(a) | static_cast<int>(b));}\
__VA_ARGS__ enum_type operator&(enum_type a, enum_type b){ return static_cast<enum_type>(static_cast<int>(a) & static_cast<int>(b));};\
__VA_ARGS__ enum_type operator~(enum_type a){ return static_cast<enum_type>(~static_cast<int>(a));}\
__VA_ARGS__ enum_type& operator|=(enum_type& a, enum_type b) { a = a | b; return a; }\
__VA_ARGS__ enum_type& operator&=(enum_type& a, enum_type b) { a = a & b; return a; }\
__VA_ARGS__ chaos::EnumBitmaskMetaData<enum_type> const * GetEnumBitmaskMetaData(boost::mpl::identity<enum_type>)\
{\
	return bitmask_metadata;\
}\
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
}\
__VA_ARGS__ bool AreValidFlags(enum_type src)\
{\
	return GetEnumBitmaskMetaData(boost::mpl::identity<enum_type>())->AreValidFlags(src);\
}

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_DECLARE_ENUM_METHOD(enum_type, ...)\
__VA_ARGS__ bool StringToEnum(char const * src, enum_type& dst);\
__VA_ARGS__ char const * EnumToString(enum_type src);\
__VA_ARGS__ char const * EnumToString(enum_type src, char * buffer, size_t buflen);\
__VA_ARGS__ chaos::EnumMetaData<enum_type> const * GetEnumMetaData(boost::mpl::identity<enum_type>);\
__VA_ARGS__ std::istream & operator >> (std::istream& stream, enum_type& dst);\
__VA_ARGS__ std::ostream & operator << (std::ostream& stream, enum_type src);

/** you may use an additionnal argument to represent the function API (CHAOS_API for example) */
#define CHAOS_IMPLEMENT_ENUM_METHOD(enum_type, metadata, ...)\
__VA_ARGS__ bool StringToEnum(char const * src, enum_type& dst)\
{\
	return (metadata)->StringToValue(src, dst);\
}\
__VA_ARGS__ char const * EnumToString(enum_type src)\
{\
	return (metadata)->ValueToString(src, nullptr, 0);\
}\
__VA_ARGS__ char const * EnumToString(enum_type src, char * buffer, size_t buflen)\
{\
	return (metadata)->ValueToString(src, buffer, buflen);\
}\
__VA_ARGS__ chaos::EnumMetaData<enum_type> const * GetEnumMetaData(boost::mpl::identity<enum_type>)\
{\
	if ((metadata) && (metadata)->IsValid())\
		return (metadata);\
	return nullptr;\
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
	char buffer[256];\
	stream << EnumToString(src, buffer, 256);\
	return stream;\
}

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace chaos
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
		EnumMetaData(std::initializer_list<EnumMetaDataEntry<T>> in_entries):
			entries(in_entries)
		{}

		/** check whether there is at least one entries */
		bool IsValid() const
		{
			return (entries.size() > 0);
		}

		/** gets the number of entry */
		size_t GetEntryCount() const
		{
			return entries.size();
		}

		/** gets the meta data at a given position */
		EnumMetaDataEntry<T> const& GetEntry(size_t index) const
		{
			assert(index < entries.size());
			return entries[index];
		}

		/** search an entry by name */
		EnumMetaDataEntry<T> const * FindEntryByName(char const* src) const
		{
			for (EnumMetaDataEntry<T> const & entry : entries)
				if (chaos::StringTools::Stricmp(src, entry.name) == 0)
					return &entry;
			return nullptr;
		}

		/** search an entry by value */
		EnumMetaDataEntry<T> const * FindEntryByValue(T src) const
		{
			for (EnumMetaDataEntry<T> const & entry : entries)
				if (src == entry.value)
					return &entry;
			return nullptr;
		}

		/** convert string into value */
		bool StringToValue(char const* src, T & dst) const
		{
			// normal enums
			if constexpr (!IsEnumBitmask_v<T>)
			{
				if (EnumMetaDataEntry<T> const * entry = FindEntryByName(src))
				{
					dst = entry->value;
					return true;
				}
			}
			// bitmask enums
			else
			{
				T other_dst = T(0); // work on a copy because the string could produce an invalid value

				// construct enum value from string fragments
				bool error = StringTools::WithSplittedText(src, "|", [this, &other_dst](char const* subkey)
				{
					if (EnumMetaDataEntry<T> const * entry = FindEntryByName(subkey))
					{
						other_dst |= entry->value;
						return false;
					}
					return true; // error
				});

				if (!error && AreValidFlags(other_dst))
				{
					dst = other_dst;
					return true;
				}
			}
			return false;
		}

		/** convert value into string */
		char const * ValueToString(T value, char * buffer = nullptr, size_t buflen = 0) const
		{
			int value_as_int = static_cast<int>(value);

			if ((!IsEnumBitmask_v<T>) ||               // standard enum -> normal search by name
				(value_as_int == 0)   ||               // this is a bitfield enum, but no bit set -> normal search by name
				(MathTools::IsPowerOf2(value_as_int))) // this is a bitfield enum, but a single bit set -> normal search by name
			{
				if (EnumMetaDataEntry<T> const * entry = FindEntryByValue(value))
					return entry->name;
			}
			else // bitfield with more than one bit -> try to reconstitute the string
			{
				assert(buffer != nullptr);
				assert(buflen > 0);

				std::ostrstream stream(buffer, buflen - 1); // -1 to keep at least zero terminal

				bool error = BitTools::ForEachBitForward(value_as_int, [this, &stream](int index)
				{
					T bit_as_value = static_cast<T>(1 << index);

					if (EnumMetaDataEntry<T> const * entry = FindEntryByValue(bit_as_value))
					{
						if (stream.pcount() > 0)
							stream << "|";
						stream << entry->name;
						return false;
					}
					return true; // error
				});

				if (!error)
				{
					buffer[stream.pcount()] = 0; // terminal zero
					return buffer;
				}
			}
			return nullptr;
		}

		/** enumerate all possible values */
		template<typename FUNC>
		decltype(auto) ForEachEnumValue(FUNC const& func) const
		{
			using L = meta::LambdaInfo<FUNC, T>;

			for (EnumMetaDataEntry<T> const & entry : entries)
			{
				if constexpr (L::convertible_to_bool)
				{
					if (decltype(auto) result = func(entry.value))
						return result;
				}
				else
				{
					func(entry.value);
				}
			}

			if constexpr (L::convertible_to_bool)
				return typename L::result_type {};
		}

	public:

		/** the enum entries info */
		std::vector<EnumMetaDataEntry<T>> entries;
	};

	/**
	 * EnumBitmaskMetaData: some extra data dedicated to bitmask enums
	 */

	template<typename T>
	class EnumBitmaskMetaData
	{
		/** a structure to store groups in same vector */
		union IncompatibilityGroupEntry
		{
			size_t group_size; // first element of a group is the number of values it belongs
			T      value;      // following elements of a group are the values themselves
		};

	public:

		/** constructor */
		EnumBitmaskMetaData(std::initializer_list<std::initializer_list<T>> entries)
		{
			// count the total number of entry required
			size_t count = 0;
			for (std::initializer_list<T> const& entry : entries)
				count += 1 + entry.size(); // +1 for the group counter
			incompatibility_groups.reserve(count);

			// splat the array of arrays into a single vector
			for (std::initializer_list<T> const& entry : entries) 
			{
				// insert the number of element in the group
				IncompatibilityGroupEntry group_entry;
				group_entry.group_size = entry.size();
				incompatibility_groups.push_back(group_entry);
				// insert all elements in the group
				for (T value : entry)
				{
					IncompatibilityGroupEntry group_entry;
					group_entry.value = value;
					incompatibility_groups.push_back(group_entry);
				}
			}
		}

		/** request for incompatible values */
		template<typename FUNC>
		decltype(auto) ForEachIncompatibleValue(T value, FUNC const& func) const
		{
			using L = meta::LambdaInfo<FUNC, T>;

			size_t count = incompatibility_groups.size();
			for (size_t i = 0; i < count; ) // do not ++i. this is handled by the loop itself
			{
				size_t group_size = incompatibility_groups[i].group_size;

				// search whether this group contains the searched value
				bool group_contains_value = false;
				for (size_t j = 0; j < group_size && !group_contains_value ; ++j)
					if (incompatibility_groups[i + 1 + j].value == value)
						group_contains_value = true;

				// if group contains value, apply incompatibilities
				if (group_contains_value)
				{
					for (size_t j = 0; j < group_size; ++j)
					{
						T other_value = incompatibility_groups[i + 1 + j].value;
						if (other_value != value)
						{
							if constexpr (L::convertible_to_bool)
							{
								if (decltype(auto) result = func(other_value))
									return result;
							}
							else
							{
								func(other_value);
							}
						}
					}
				}
				// next group
				i = i + 1 + group_size;
			}

			if constexpr (L::convertible_to_bool)
				return typename L::result_type{};
		}

		/** check whether there is some flags that are incompatible */
		bool AreValidFlags(T value) const
		{
			size_t count = incompatibility_groups.size();
			for (size_t i = 0; i < count; ) // do not ++i. this is handled by the loop itself
			{
				size_t group_size = incompatibility_groups[i].group_size;

				size_t flags_in_group_count = 0; // number of flag in this group
				for (size_t j = 0; j < group_size; ++j)
				{
					T other_value = incompatibility_groups[i + 1 + j].value;

					if (HasAnyFlags(value, other_value))
						if (++flags_in_group_count >= 2) // incomming value has 2 flags from incompatible group -> error
							return false;
				}
				// next group
				i = i + 1 + group_size;
			}
			return true;
		}

	protected:

		// incompatibility groups are formed as follow:
		// -a single size_t indicating the number of elements in the group
		// -N values that are incompatible one with the other

		/** the incompatibility groups */
		std::vector<IncompatibilityGroupEntry> incompatibility_groups;
	};

}; // namespace chaos

#endif