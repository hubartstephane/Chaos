namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<bool WRITABLE>
	class JSONConfiguration;

	using JSONReadConfiguration = JSONConfiguration<false>;
	using JSONWriteConfiguration = JSONConfiguration<true>;

	namespace JSONTools
	{
		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetNode(JSONConfiguration<WRITABLE> const& config, PARAMS&& ...params);

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetStructureNode(JSONConfiguration<WRITABLE> const& config, PARAMS&& ...params);

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetObjectNode(JSONConfiguration<WRITABLE> const& config, PARAMS&& ...params);

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetArrayNode(JSONConfiguration<WRITABLE> const& config, PARAMS&& ...params);

		template<bool WRITABLE, typename FUNC>
		decltype(auto) ForEachSource(JSONConfiguration<WRITABLE> const& config, FUNC const& func);

		template<typename T>
		bool SetAttribute(JSONWriteConfiguration const& config, std::string_view path, T const& src);

		template<typename T>
		bool SetAttributeByIndex(JSONWriteConfiguration const& config, size_t index, T const& src);
	}

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * JSONConfiguration: a combinaison of configuration/persistent data where you can read and write data
	 */
	template<bool WRITABLE>
	class JSONConfiguration
	{
	public:

		/** default constructor */
		JSONConfiguration() = default;
		/** copy constructor */
		JSONConfiguration(JSONConfiguration const &) = default;
		/** move constructor */
		JSONConfiguration(JSONConfiguration &&) = default;
		/** conversion constructor */
		JSONConfiguration(nlohmann::json const * in_default_config) : default_config(in_default_config) {};

		/** check whether there is at least one valid data */
		operator bool() const
		{
			return (default_config != nullptr) || (persistent_config != nullptr);
		}
		/** conversion operator */
		operator nlohmann::json const* () const
		{
			return default_config;
		}

	public:

		/** the normal json object */
		nlohmann::json const* default_config = nullptr;
		/** the persistent json object */
		std::conditional<WRITABLE, nlohmann::json, nlohmann::json const>::type * persistent_config = nullptr;
	};

	namespace JSONTools
	{
		template<bool WRITABLE, typename FUNC>
		decltype(auto) ForEachSource(JSONConfiguration<WRITABLE> const & config, FUNC const& func)
		{
			using L = meta::LambdaInfo<FUNC, nlohmann::json const *>;

			if constexpr (L::convertible_to_bool)
			{
				if (config.persistent_config != nullptr)
					if (decltype(auto) result = func(config.persistent_config))
						return result;
				if (config.default_config != nullptr)
					if (decltype(auto) result = func(config.default_config))
						return result;
				return typename L::result_type{};
			}
			else
			{
				if (config.persistent_config != nullptr)
					func(config.persistent_config);
				if (config.default_config != nullptr)
					func(config.default_config);
			}
		}

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetNode(JSONConfiguration<WRITABLE> const& config, PARAMS&& ...params) // READ-ONLY: creation as no sense
		{
			JSONConfiguration<WRITABLE> result;
			result.default_config = JSONTools::GetNode(config.default_config, std::forward<PARAMS>(params)...);
			result.persistent_config = JSONTools::GetNode(config.persistent_config, std::forward<PARAMS>(params)...);
			return result;
		}

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetStructureNode(JSONConfiguration<WRITABLE> const& config, PARAMS&& ...params) // READ-ONLY: creation as no sense
		{
			JSONConfiguration<WRITABLE> result;
			result.default_config = JSONTools::GetStructureNode(config.default_config, std::forward<PARAMS>(params)...);
			result.persistent_config = JSONTools::GetStructureNode(config.persistent_config, std::forward<PARAMS>(params)...);
			return result;
		}

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetObjectNode(JSONConfiguration<WRITABLE> const & config, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.default_config = JSONTools::GetObjectNode(config.default_config, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.persistent_config = JSONTools::GetOrCreateObjectNode(config.persistent_config, std::forward<PARAMS>(params)...);
			else
				result.persistent_config = JSONTools::GetObjectNode(config.persistent_config, std::forward<PARAMS>(params)...);

			return result;
		}

		template<bool WRITABLE, typename ...PARAMS>
		JSONConfiguration<WRITABLE> GetArrayNode(JSONConfiguration<WRITABLE> const & config, PARAMS&& ...params)
		{
			JSONConfiguration<WRITABLE> result;
			result.default_config = JSONTools::GetArrayNode(config.default_config, std::forward<PARAMS>(params)...);

			if constexpr (WRITABLE)
				result.persistent_config = JSONTools::GetOrCreateArrayNode(config.persistent_config, std::forward<PARAMS>(params)...);
			else
				result.persistent_config = JSONTools::GetArrayNode(config.persistent_config, std::forward<PARAMS>(params)...);

			return result;
		}

		template<typename T>
		bool SetAttribute(JSONWriteConfiguration const & config, std::string_view path, T const& src)
		{
			if (config.persistent_config == nullptr)
				return false;
			return SetAttribute(config.persistent_config, path, src);
		}

		template<typename T>
		bool SetAttributeByIndex(JSONWriteConfiguration const & config, size_t index, T const& src)
		{
			if (config.persistent_config == nullptr)
				return false;
			return SetAttributeByIndex(config.persistent_config, index, src);
		}

	}; // namespace JSONTools

#endif

}; // namespace chaos