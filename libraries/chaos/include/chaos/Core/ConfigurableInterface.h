namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	enum class ReadConfigurablePropertiesContext;

	class ConfigurableInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	*  ReadConfigurablePropertiesContext: the situation when readding properties is done
	*/

	enum class ReadConfigurablePropertiesContext : int
	{
		INITIALIZATION,
		HOT_RELOAD
	};

	/**
	* ConfigurableInterface: an interface representing that can have a configuration. Theses objects may react to confgiuration updates
	*/

	class CHAOS_API ConfigurableInterface
	{
		friend class ObjectConfigurationBase;

	public:

		/** destructor */
		virtual ~ConfigurableInterface() = default;

		/** getter on the configuration */
		ObjectConfigurationBase const * GetObjectConfiguration() const
		{
			return configuration.get();
		}

		ObjectConfigurationBase * GetObjectConfiguration()
		{
			return configuration.get();
		}

		/** set the configuration of the object */
		void SetObjectConfiguration(ObjectConfigurationBase* in_configuration);

		/** get the interface used for reading */
		JSONReadConfiguration GetJSONReadConfiguration() const;
		/** get the interface used for reading & writing */
		JSONWriteConfiguration GetJSONWriteConfiguration() const;

		/** create a child configuration and give it to another object */
		bool GiveChildConfiguration(ConfigurableInterface* other_configurable, std::string key) const;
		/** read the properties (an children) from the config */
		bool ReadConfigurableProperties(ReadConfigurablePropertiesContext context, bool recurse);
		/** store the persistent properties (and children) into JSON (no disk access) */
		bool StorePersistentProperties(bool recurse) const;
		/** save persistent properties to file (whole hierarchy) */
		bool SavePersistentPropertiesToFile(bool store_properties) const;
		/** read config from files (whole hierarchy) */
		bool LoadConfigurablePropertiesFromFile(FilePathParam const& in_default_config_path, FilePathParam const& in_persistent_config_path, bool send_notifications);
		/** reload default properties (from root or not) */
		bool ReloadDefaultPropertiesFromFile(bool partial_reload_only, bool send_notifications);

	protected:

		/** called whenever the configuration has been changed */
		virtual bool OnConfigurationChanged(JSONReadConfiguration config);
		/** the reading properties effective method */
		virtual bool OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context);
		/** the storing persistent properties effective method (no disk access) */
		virtual bool OnStorePersistentProperties(JSONWriteConfiguration config) const;

	protected:

		/** the configuration */
		shared_ptr<ObjectConfigurationBase> configuration;
	};

#endif

}; // namespace chaos