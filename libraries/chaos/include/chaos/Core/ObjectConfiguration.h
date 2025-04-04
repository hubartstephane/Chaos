namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class ObjectConfigurationBase;
	class ChildObjectConfiguration;
	class RootObjectConfiguration;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ObjectConfigurationBase: some configuration that may be used for reading and writing (for persistent data)
	*/

	class CHAOS_API ObjectConfigurationBase : public Object
	{
		friend class ConfigurableInterface;
		friend class ChildObjectConfiguration;
		friend class RootObjectConfiguration;

	public:

		/** get the target configurable object */
		ConfigurableInterface* GetConfigurable();
		/** get the target configurable object */
		ConfigurableInterface const* GetConfigurable() const;

		/** get the interface used for reading */
		JSONReadConfiguration GetJSONReadConfiguration() const;
		/** get the interface used for reading & writing*/
		JSONWriteConfiguration GetJSONWriteConfiguration() const;

		/** gets the root configuration */
		virtual RootObjectConfiguration* GetRootConfiguration();
		/** gets the root configuration */
		virtual RootObjectConfiguration const* GetRootConfiguration() const;

		/** create a configuration that is clone of us */
		virtual ObjectConfigurationBase* CreateClonedConfiguration();
		/** create a child configuration */
		ChildObjectConfiguration* CreateChildConfiguration(std::string path);

		/** recursively send notifications to ConfigurableObjects */
		void PropagateNotificationToUsers();

		/** reload the default configuration part. Only affect current node and its children value (not its parents nor siblings nodes) */
		virtual bool ReloadDefaultPropertiesFromFile(ReloadConfigurationMode load_mode, bool send_notifications);

		/** read the properties (an children) from the config */
		bool ReadConfigurableProperties(ReadConfigurablePropertiesContext context, bool recurse);
		/** store the persistent properties (and children) into JSON (no disk access) */
		bool StorePersistentProperties(bool recurse) const;

	protected:

		/** call this whenever the configuration is being changed */
		void CompleteUpdateOperation(bool send_notifications);
		/** helper function used to reload the hierarchy */
		virtual nlohmann::json const* ReloadHelper(nlohmann::json& new_root_storage, std::string_view in_path);

	protected:

		/** cannot be construct except by CreateChildConfiguration(...) */
		ObjectConfigurationBase() = default;

		/** the children configurations */
		std::vector<shared_ptr<ChildObjectConfiguration>> child_configurations;
		/** the configurable object owning this */
		weak_ptr<Object> configurable_object;

		/** the json node to read info from */
		nlohmann::json* default_config = nullptr;
		/** the json node to persistent info into */
		nlohmann::json* persistent_config = nullptr;

		/** the storage for json node to read info from */
		nlohmann::json storage_default_config;
	};


	/**
	* ChildObjectConfiguration: a specialization that has a parent node and whose content depend on that
	*/

	class CHAOS_API ChildObjectConfiguration : public ObjectConfigurationBase
	{
		friend class ObjectConfigurationBase;
		friend class RootObjectConfiguration;

	public:

		/** destructor */
		virtual ~ChildObjectConfiguration();
		/** override */
		virtual void SubReference() override;

		/** override */
		virtual ObjectConfigurationBase* CreateClonedConfiguration() override;
		/** override */
		virtual RootObjectConfiguration* GetRootConfiguration() override;
		/** override */
		virtual RootObjectConfiguration const* GetRootConfiguration() const override;
		/** override */
		virtual bool ReloadDefaultPropertiesFromFile(ReloadConfigurationMode load_mode, bool send_notifications) override;

	protected:

		/** override */
		virtual nlohmann::json const* ReloadHelper(nlohmann::json& new_root_storage, std::string_view in_path) override;

		/** cannot be construct except by CreateChildConfiguration(...) */
		ChildObjectConfiguration() = default;

		/** recursively update child configuration */
		void PropagateInternalConfigsUpdates();
		/** update json nodes from parent configuration */
		void UpdateInternalConfigsFromParent();
		/** remove the configuration from its parent */
		void RemoveFromParent();

	protected:

		/** the parent configuration */
		weak_ptr<ObjectConfigurationBase> parent_configuration;
		/** the path from parent to this configuration */
		std::string path;
	};

	/**
	* RootObjectConfiguration: a specialization store the whole JSON hierarchy and lend sub-content to its children
	*/

	class CHAOS_API RootObjectConfiguration : public ObjectConfigurationBase
	{
		friend class ObjectConfigurationBase;
		friend class ChildObjectConfiguration;

	public:

		/** constructor */
		RootObjectConfiguration();

		/** change the default config path */
		void SetDefaultConfigurationPath(FilePathParam const& in_default_config_path);
		/** change the persistent config path */
		void SetPersistentConfigurationPath(FilePathParam const& in_persistent_config_path);

		/** read config from files (whole hierarchy) */
		bool LoadConfigurablePropertiesFromFile(FilePathParam const& in_default_config_path, FilePathParam const& in_persistent_config_path, bool send_notifications);
		/** save the persistent data (whole hierarchy) */
		bool SavePersistentPropertiesToFile(bool store_properties) const;

		/** override */
		virtual ObjectConfigurationBase* CreateClonedConfiguration() override;
		/** override */
		virtual RootObjectConfiguration* GetRootConfiguration() override;
		/** override */
		virtual RootObjectConfiguration const* GetRootConfiguration() const override;
		/** override */
		virtual bool ReloadDefaultPropertiesFromFile(ReloadConfigurationMode load_mode, bool send_notifications) override;

	protected:

		/** override */
		virtual nlohmann::json const* ReloadHelper(nlohmann::json& new_root_storage, std::string_view in_path) override;
		/** internal method to load configuration from file */
		bool DoLoadConfigurablePropertiesFromFile(bool load_default, bool load_persistent, bool send_notifications);

	protected:

		/** the path for the default configuration */
		boost::filesystem::path default_config_path;
		/** the path for the persistent configuration */
		boost::filesystem::path persistent_config_path;

		/** the json storage for persistent data */
		nlohmann::json storage_persistent_config;
	};

#endif

}; // namespace chaos