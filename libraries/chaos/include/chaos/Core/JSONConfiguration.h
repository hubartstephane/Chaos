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
	template<>
	class JSONConfiguration<false>
	{
	public:

		/** the normal json object */
		nlohmann::json const* read_config = nullptr;
		/** the persistent json object */
		nlohmann::json const* write_config = nullptr;
	};

	template<>
	class JSONConfiguration<true>
	{
	public:

		/** the normal json object */
		nlohmann::json const* read_config = nullptr;
		/** the persistent json object */
		nlohmann::json* write_config = nullptr;
	};

	template<typename ...PARAMS>
	bool SaveIntoJSON(JSONWriteConfiguration entry, PARAMS&& ...params)
	{
		if (entry.write_config == nullptr)
			return false;
		return SaveIntoJSON(*entry.write_config, std::forward<PARAMS>(params)...);
	}

	namespace JSONTools
	{
		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetStructure(JSONConfiguration<WRITABLE> entry, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.read_config = JSONTools::GetStructure(*entry.read_config, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.write_config = JSONTools::GetOrCreateStructure(*entry.write_config, std::forward<PARAMS>(params)...);
			else
				result.write_config = JSONTools::GetStructure(*entry.write_config, std::forward<PARAMS>(params)...);

			return result;
		}

		template<bool WRITABLE, typename T>
		bool GetAttribute(JSONConfiguration<WRITABLE> entry, char const* name, T& result)
		{
			if (entry.write_config != nullptr)
				if (GetAttribute(*entry.write_config, name, result))
					return true;
			if (entry.read_config != nullptr)
				if (GetAttribute(*entry.read_config, name, result))
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
			if (entry.write_config != nullptr)
			{
				if (entry.write_config->is_array())
				{
					if (index >= entry.write_config->size())
						return false;
					return LoadFromJSON(entry.write_config->operator [](index), result);
				}
			}
			if (entry.read_config != nullptr)
			{
				if (entry.read_config->is_array())
				{
					if (index >= entry.read_config->size())
						return false;
					return LoadFromJSON(entry.read_config->operator [](index), result);
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
			assert(entry.write_config != nullptr);
			if (entry.write_config->is_null())
				*entry.write_config = nlohmann::json::object();
			else if (!entry.write_config->is_object())
				return false;
			entry.write_config->operator [](name) = nlohmann::json();
			return SaveIntoJSON(entry.write_config->operator[](name), src);
		}

		template<typename T>
		bool SetAttributeByIndex(JSONWriteConfiguration entry, size_t index, T const& src)
		{
			assert(entry.write_config != nullptr);
			if (entry.write_config->is_null())
				*entry.write_config = nlohmann::json::array();
			else if (!entry.write_config->is_array())
				return false;
			entry.write_config->operator[](index) = nlohmann::json();
			return SaveIntoJSON(entry.write_config->operator[](index), src);
		}

	}; // namespace JSONTools

#endif

}; // namespace chaos