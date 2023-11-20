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
		std::conditional<WRITABLE, nlohmann::json, nlohmann::json const>::type * persistent_config = nullptr;
	};

	template<typename ...PARAMS>
	bool SaveIntoJSON(JSONWriteConfiguration config, PARAMS&& ...params)
	{
		if (config.persistent_config == nullptr)
			return false;
		return SaveIntoJSON(*config.persistent_config, std::forward<PARAMS>(params)...);
	}

	namespace JSONTools
	{

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetObjectNode(JSONConfiguration<WRITABLE> config, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.default_config = JSONTools::GetObjectNode(*config.default_config, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.persistent_config = JSONTools::GetOrCreateObjectNode(*config.persistent_config, std::forward<PARAMS>(params)...);
			else
				result.persistent_config = JSONTools::GetObjectNode(*config.persistent_config, std::forward<PARAMS>(params)...);

			return result;
		}

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetArrayNode(JSONConfiguration<WRITABLE> config, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.default_config = JSONTools::GetArrayNode(*config.default_config, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.persistent_config = JSONTools::GetOrCreateArrayNode(*config.persistent_config, std::forward<PARAMS>(params)...);
			else
				result.persistent_config = JSONTools::GetArrayNode(*config.persistent_config, std::forward<PARAMS>(params)...);

			return result;
		}

		template<typename ...PARAMS>
		JSONReadConfiguration GetStructureNode(JSONReadConfiguration config, PARAMS&& ...params) // READ-ONLY: creation as no sense
		{
			JSONReadConfiguration result;
			result.default_config = JSONTools::GetStructureNode(*config.default_config, std::forward<PARAMS>(params)...);
			result.persistent_config = JSONTools::GetStructureNode(*config.persistent_config, std::forward<PARAMS>(params)...);
			return result;
		}

		template<bool WRITABLE, typename T>
		bool GetAttribute(JSONConfiguration<WRITABLE> config, std::string_view path, T& result)
		{
			return config.ForEachSource([path, &result](nlohmann::json const & json)
			{
				return GetAttribute(json, path, result);
			});
		}

		template<bool WRITABLE, typename T, typename Y>
		bool GetAttribute(JSONConfiguration<WRITABLE> config, std::string_view path, T& result, Y default_value)
		{
			if (GetAttribute(config, path, result))
				return true;
			result = default_value;
			return false;
		}

		template<bool WRITABLE, typename T>
		bool GetAttributeByIndex(JSONConfiguration<WRITABLE> config, size_t index, T& result)
		{
			return config.ForEachSource([index, &result](nlohmann::json const & json)
			{
				return GetAttributeByIndex(json, index, result);
			});
		}

		template<bool WRITABLE, typename T, typename Y>
		bool GetAttributeByIndex(JSONConfiguration<WRITABLE> config, size_t index, T& result, Y default_value)
		{
			if (GetAttributeByIndex(config, index, result))
				return true;
			result = default_value;
			return false;
		}

		template<typename T>
		bool SetAttribute(JSONWriteConfiguration config, std::string_view path, T const& src)
		{
			assert(config.persistent_config != nullptr);
			if (nlohmann::json * node = GetOrCreateNode(*config.persistent_config, path))
				return SaveIntoJSON(*node, src);
			return false;
		}

		template<typename T>
		bool SetAttributeByIndex(JSONWriteConfiguration config, size_t index, T const& src)
		{
			assert(config.persistent_config != nullptr);
			if (config.persistent_config->is_null())
				*config.persistent_config = nlohmann::json::array();
			else if (!config.persistent_config->is_array())
				return false;
			config.persistent_config->operator[](index) = nlohmann::json();
			return SaveIntoJSON(config.persistent_config->operator[](index), src);
		}

	}; // namespace JSONTools

#endif

}; // namespace chaos