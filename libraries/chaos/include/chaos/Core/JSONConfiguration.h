namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<bool WRITABLE>
	class JSONConfiguration;

	using JSONReadConfiguration = JSONConfiguration<false>;
	using JSONWriteConfiguration = JSONConfiguration<true>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * JSONConfiguration: a combinaison of configuration/persistent data where you can read and write data
	 */
	template<bool WRITABLE>
	class JSONConfiguration
	{
	public:

		/** the normal json object */
		nlohmann::json const* normal_configuration = nullptr;
		/** the persistent json object */
		nlohmann::json* persistent_configuration = nullptr;
	};

	template<typename ...PARAMS>
	bool SaveIntoJSON(JSONWriteConfiguration entry, PARAMS&& ...params)
	{
		if (entry.persistent_configuration == nullptr)
			return false;
		return SaveIntoJSON(*entry.persistent_configuration, std::forward<PARAMS>(params)...);
	}

	namespace JSONTools
	{
		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetStructure(JSONConfiguration<WRITABLE> entry, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.normal_configuration = JSONTools::GetStructure(*entry.normal_configuration, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.persistent_configuration = JSONTools::GetOrCreateStructure(*entry.persistent_configuration, std::forward<PARAMS>(params)...);
			else
				result.persistent_configuration = JSONTools::GetStructure(*entry.persistent_configuration, std::forward<PARAMS>(params)...);

			return result;
		}

		template<bool WRITABLE, typename T>
		bool GetAttribute(JSONConfiguration<WRITABLE> entry, char const* name, T& result)
		{
			if (entry.persistent_configuration != nullptr)
				if (GetAttribute(*entry.persistent_configuration, name, result))
					return true;
			if (entry.normal_configuration != nullptr)
				if (GetAttribute(*entry.normal_configuration, name, result))
					return true;
			return false;
		}

		template<bool WRITABLE, typename T, typename Y>
		bool GetAttribute(JSONConfiguration<WRITABLE> entry, char const* name, T& result, Y default_value)
		{
			if (GetAttribute(entry, name, result))
				return true;
			result = default_value;
			return false;
		}

		template<bool WRITABLE, typename T>
		bool GetAttributeByIndex(JSONConfiguration<WRITABLE> entry, size_t index, T& result)
		{
			if (entry.persistent_configuration != nullptr)
			{
				if (entry.persistent_configuration->is_array())
				{
					if (index >= entry.persistent_configuration->size())
						return false;
					return LoadFromJSON(entry.persistent_configuration->operator [](index), result);
				}
			}
			if (entry.normal_configuration != nullptr)
			{
				if (entry.normal_configuration->is_array())
				{
					if (index >= entry.normal_configuration->size())
						return false;
					return LoadFromJSON(entry.normal_configuration->operator [](index), result);
				}
			}
			return false;
		}

		template<bool WRITABLE, typename T, typename Y>
		bool GetAttributeByIndex(JSONConfiguration<WRITABLE> entry, size_t index, T& result, Y default_value)
		{
			if (GetAttributeByIndex(entry, index, result))
				return true;
			result = default_value;
			return false;
		}

		template<typename T>
		bool SetAttribute(JSONWriteConfiguration entry, char const* name, T const& src)
		{
			assert(name != nullptr);
			assert(entry.persistent_configuration != nullptr);
			if (entry.persistent_configuration->is_null())
				*entry.persistent_configuration = nlohmann::json::object();
			else if (!entry.persistent_configuration->is_object())
				return false;
			entry.persistent_configuration->operator [](name) = nlohmann::json();
			return SaveIntoJSON(entry.persistent_configuration->operator[](name), src);
		}

		template<typename T>
		bool SetAttributeByIndex(JSONWriteConfiguration entry, size_t index, T const& src)
		{
			assert(entry.persistent_configuration != nullptr);
			if (entry.persistent_configuration->is_null())
				*entry.persistent_configuration = nlohmann::json::array();
			else if (!entry.persistent_configuration->is_array())
				return false;
			entry.persistent_configuration->operator[](index) = nlohmann::json();
			return SaveIntoJSON(entry.persistent_configuration->operator[](index), src);
		}

	}; // namespace JSONTools

#endif

}; // namespace chaos