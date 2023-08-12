namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

#if 0
	template<bool WRITABLE>
	class JSONConfiguration;

	using JSONReadConfiguration = JSONConfiguration<false>;
	using JSONWriteConfiguration = JSONConfiguration<true>;
#endif

	class JSONReadConfiguration;
	class JSONWriteConfiguration;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * JSONReadConfiguration: a combinaison of configuration/persistent data where you can only read data
	 */
	class JSONReadConfiguration

	{
	public:

		/** the normal json object */
		nlohmann::json const* normal_configuration = nullptr;
		/** the persistent json object */
		nlohmann::json const* persistent_configuration = nullptr;
	};

	/**
	 * JSONReadConfiguration: a combinaison of configuration/persistent data where you can read and write data
	 */
	class JSONWriteConfiguration
	{
	public:

		/** the normal json object */
		nlohmann::json const* normal_configuration = nullptr;
		/** the persistent json object */
		nlohmann::json* persistent_configuration = nullptr;
	};

#endif

}; // namespace chaos