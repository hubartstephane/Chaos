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
		JSONConfiguration<WRITABLE> GetObjectNode(JSONConfiguration<WRITABLE> entry, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.read_config = JSONTools::GetObjectNode(*entry.read_config, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.write_config = JSONTools::GetOrCreateObjectNode(*entry.write_config, std::forward<PARAMS>(params)...);
			else
				result.write_config = JSONTools::GetObjectNode(*entry.write_config, std::forward<PARAMS>(params)...);

			return result;
		}

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetArrayNode(JSONConfiguration<WRITABLE> entry, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.read_config = JSONTools::GetArrayNode(*entry.read_config, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.write_config = JSONTools::GetOrCreateArrayNode(*entry.write_config, std::forward<PARAMS>(params)...);
			else
				result.write_config = JSONTools::GetArrayNode(*entry.write_config, std::forward<PARAMS>(params)...);

			return result;
		}

		template<typename ...PARAMS>
		JSONReadConfiguration GetStructureNode(JSONReadConfiguration entry, PARAMS&& ...params) // READ-ONLY: creation as no sense
		{
			JSONReadConfiguration result;
			result.read_config = JSONTools::GetStructureNode(*entry.read_config, std::forward<PARAMS>(params)...);
			result.write_config = JSONTools::GetStructureNode(*entry.write_config, std::forward<PARAMS>(params)...);
			return result;
		}

		template<bool WRITABLE, typename T>
		bool GetAttribute(JSONConfiguration<WRITABLE> entry, std::string_view path, T& result)
		{
			if (entry.write_config != nullptr)
				if (GetAttribute(*entry.write_config, path, result))
					return true;
			if (entry.read_config != nullptr)
				if (GetAttribute(*entry.read_config, path, result))
					return true;
			return false;
		}

		template<bool WRITABLE, typename T, typename Y>
		bool GetAttribute(JSONConfiguration<WRITABLE> entry, std::string_view path, T& result, Y default_value)
		{
			if (GetAttribute(entry, path, result))
				return true;
			result = default_value;
			return false;
		}

		template<bool WRITABLE, typename T>
		bool GetAttributeByIndex(JSONConfiguration<WRITABLE> entry, size_t index, T& result)
		{
			if (entry.write_config != nullptr)
				if (GetAttributeByIndex(*entry.write_config, index, result))
					return true;
			if (entry.read_config != nullptr)
				if (GetAttributeByIndex(*entry.read_config, index, result))
					return true;
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
		bool SetAttribute(JSONWriteConfiguration entry, std::string_view path, T const& src)
		{
			assert(entry.write_config != nullptr);
			if (nlohmann::json * node = GetOrCreateNode(*entry.write_config, path))
				return SaveIntoJSON(*node, src);
			return false;
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