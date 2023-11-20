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

		template<typename FUNC>
		decltype(auto) ForEachSource(FUNC const& func) const
		{
			using L = meta::LambdaInfo<FUNC, nlohmann::json const&>;

			if constexpr (L::convertible_to_bool)
			{
				if (persistent_config != nullptr)
					if (decltype(auto) result = func(*persistent_config))
						return result;
				if (default_config != nullptr)
					if (decltype(auto) result = func(*default_config))
						return result;
				return typename L::result_type{};
			}
			else
			{
				if (persistent_config != nullptr)
					func(*persistent_config);
				if (default_config != nullptr)
					func(*default_config);
			}
		}

	public:

		/** the normal json object */
		nlohmann::json const* default_config = nullptr;
		/** the persistent json object */
		boost::mpl::if_c<WRITABLE, nlohmann::json, nlohmann::json const>::type * persistent_config = nullptr;
	};

	template<typename ...PARAMS>
	bool SaveIntoJSON(JSONWriteConfiguration entry, PARAMS&& ...params)
	{
		if (entry.persistent_config == nullptr)
			return false;
		return SaveIntoJSON(*entry.persistent_config, std::forward<PARAMS>(params)...);
	}

	namespace JSONTools
	{

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetObjectNode(JSONConfiguration<WRITABLE> entry, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.default_config = JSONTools::GetObjectNode(*entry.default_config, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.persistent_config = JSONTools::GetOrCreateObjectNode(*entry.persistent_config, std::forward<PARAMS>(params)...);
			else
				result.persistent_config = JSONTools::GetObjectNode(*entry.persistent_config, std::forward<PARAMS>(params)...);

			return result;
		}

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetArrayNode(JSONConfiguration<WRITABLE> entry, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.default_config = JSONTools::GetArrayNode(*entry.default_config, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.persistent_config = JSONTools::GetOrCreateArrayNode(*entry.persistent_config, std::forward<PARAMS>(params)...);
			else
				result.persistent_config = JSONTools::GetArrayNode(*entry.persistent_config, std::forward<PARAMS>(params)...);

			return result;
		}

		template<typename ...PARAMS>
		JSONReadConfiguration GetStructureNode(JSONReadConfiguration entry, PARAMS&& ...params) // READ-ONLY: creation as no sense
		{
			JSONReadConfiguration result;
			result.default_config = JSONTools::GetStructureNode(*entry.default_config, std::forward<PARAMS>(params)...);
			result.persistent_config = JSONTools::GetStructureNode(*entry.persistent_config, std::forward<PARAMS>(params)...);
			return result;
		}

		template<bool WRITABLE, typename T>
		bool GetAttribute(JSONConfiguration<WRITABLE> entry, std::string_view path, T& result)
		{
			if (entry.persistent_config != nullptr)
				if (GetAttribute(*entry.persistent_config, path, result))
					return true;
			if (entry.default_config != nullptr)
				if (GetAttribute(*entry.default_config, path, result))
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
			if (entry.persistent_config != nullptr)
				if (GetAttributeByIndex(*entry.persistent_config, index, result))
					return true;
			if (entry.default_config != nullptr)
				if (GetAttributeByIndex(*entry.default_config, index, result))
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
			assert(entry.persistent_config != nullptr);
			if (nlohmann::json * node = GetOrCreateNode(*entry.persistent_config, path))
				return SaveIntoJSON(*node, src);
			return false;
		}

		template<typename T>
		bool SetAttributeByIndex(JSONWriteConfiguration entry, size_t index, T const& src)
		{
			assert(entry.persistent_config != nullptr);
			if (entry.persistent_config->is_null())
				*entry.persistent_config = nlohmann::json::array();
			else if (!entry.persistent_config->is_array())
				return false;
			entry.persistent_config->operator[](index) = nlohmann::json();
			return SaveIntoJSON(entry.persistent_config->operator[](index), src);
		}

	}; // namespace JSONTools

#endif

}; // namespace chaos