namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class ConfigurableInterfaceBase;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ConfigurableInterfaceBase: an interface representing that can have a configuration. Theses objects may react to confgiuration updates
	*/

	class CHAOS_API ConfigurableInterfaceBase
	{
		friend class ObjectConfigurationBase;

	public:

		/** destructor */
		virtual ~ConfigurableInterfaceBase() = default;

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
		/** get the interface used for reading & writing*/
		JSONWriteConfiguration GetJSONWriteConfiguration() const;

		/** reload the configuration for this object and its children */
		bool ReloadObjectConfiguration(bool send_notifications);

		/** create a child configuration and give it to another object */
		bool GiveChildConfiguration(ConfigurableInterfaceBase* other_configurable, std::string_view key);

	protected:

		/** called whenever the configuration has been changed */
		virtual void OnConfigurationChanged(JSONReadConfiguration read_config) {}

	protected:

		/** the configuration */
		shared_ptr<ObjectConfigurationBase> configuration;
	};

#endif

}; // namespace chaos