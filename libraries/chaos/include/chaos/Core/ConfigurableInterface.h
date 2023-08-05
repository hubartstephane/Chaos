namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class ConfigurableInterface;
	class ObjectConfiguration;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ConfigurableInterface: an interface representing that can have a configuration. Theses objects may react to confgiuration updates
	*/

	class ConfigurableInterface
	{
		friend class ObjectConfiguration;

	public:

		/** destructor */
		virtual ~ConfigurableInterface() = default;

		/** getter on the configuration */
		ObjectConfiguration* GetConfiguration() const
		{
			return configuration.get();
		}

		/** set the configuration of the object */
		void SetConfiguration(ObjectConfiguration* in_configuration);

	protected:

		/** called whenever the configuration has been changed */
		virtual void OnConfigurationChanged() {}

	protected:

		/** the configuration */
		shared_ptr<ObjectConfiguration> configuration;
	};


	/**
	* ObjectConfiguration: some configuration that may be used for reading and writing (for persistent data)
	*/

	class ObjectConfiguration : public Object
	{
		friend class ConfigurableInterface;

	public:

		/** destructor */
		virtual ~ObjectConfiguration();

		/** override */
		virtual void SubReference() override;

		/** create a child configuration */
		ObjectConfiguration* CreateChildConfiguration(std::string_view key);
		/** call this whenever the configuration is being changed */
		void OnConfigurationChanged();

	protected:

		/** update json nodes from parent configuration */
		void UpdateFromParent();
		/** recursively update child configuration */
		void PropagateUpdates();
		/** recursively send notifications */
		void PropagateNotifications();
		/** remove the configuration from its parent */
		void RemoveFromParent();

	protected:

		/** the json node to read info from */
		nlohmann::json* read_config = nullptr;
		/** the json node to write info into */
		nlohmann::json* write_config = nullptr;

		/** the parent configuration */
		weak_ptr<ObjectConfiguration> parent_configuration;
		/** the path from parent to this configuration */
		std::string key;
		/** the children configurations */
		std::vector<shared_ptr<ObjectConfiguration>> child_configurations;
		/** the configurable object owning this */
		weak_ptr<Object> configurable_object;
	};

#endif

}; // namespace chaos