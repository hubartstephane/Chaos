#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ---------------------------------------------------------------------
	// ConfigurableInterface implementation
	// ---------------------------------------------------------------------

	void ConfigurableInterface::SetObjectConfiguration(ObjectConfigurationBase* in_configuration)
	{
		assert(in_configuration == nullptr || in_configuration->configurable_object == nullptr); // cannot set a configuration already in use

		if (configuration.get() != in_configuration)
		{
			// erase previous configuration
			if (configuration != nullptr)
				configuration->configurable_object = nullptr;
			// set new configuration
			configuration = in_configuration;
			if (configuration != nullptr)
				configuration->configurable_object = auto_cast(this);
		}
	}

	JSONReadConfiguration ConfigurableInterface::GetJSONReadConfiguration() const
	{
		if (configuration != nullptr)
			return configuration->GetJSONReadConfiguration();
		return {};
	}

	JSONWriteConfiguration ConfigurableInterface::GetJSONWriteConfiguration() const
	{
		if (configuration != nullptr)
			return configuration->GetJSONWriteConfiguration();
		return {};
	}

	bool ConfigurableInterface::GiveClonedConfiguration(ConfigurableInterface* other_configurable) const
	{
		if (configuration != nullptr)
		{
			other_configurable->SetObjectConfiguration(configuration->CreateClonedConfiguration());
			return true;
		}
		return false;
	}

	bool ConfigurableInterface::GiveChildConfiguration(ConfigurableInterface* other_configurable, std::string key) const
	{
		if (configuration != nullptr)
		{
			other_configurable->SetObjectConfiguration(configuration->CreateChildConfiguration(std::move(key)));
			return true;
		}
		return false;
	}

	bool ConfigurableInterface::ReadConfigurableProperties(ReadConfigurablePropertiesContext context, bool recurse)
	{
		if (configuration != nullptr)
			return configuration->ReadConfigurableProperties(context, recurse);
		return false;
	}

	bool ConfigurableInterface::StorePersistentProperties(bool recurse) const
	{
		if (configuration != nullptr)
			return configuration->StorePersistentProperties(recurse);
		return false;
	}

	bool ConfigurableInterface::OnConfigurationChanged(JSONReadConfiguration config)
	{
		return OnReadConfigurableProperties(config, ReadConfigurablePropertiesContext::HOT_RELOAD);
	}

	bool ConfigurableInterface::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		return true;
	}

	bool ConfigurableInterface::OnStorePersistentProperties(JSONWriteConfiguration config) const
	{
		return true;
	}

	bool ConfigurableInterface::SavePersistentPropertiesToFile(bool store_properties) const
	{
		if (configuration != nullptr)
			if (RootObjectConfiguration* root_configuration = configuration->GetRootConfiguration())
				return root_configuration->SavePersistentPropertiesToFile(store_properties);
		return false;
	}

	bool ConfigurableInterface::LoadConfigurablePropertiesFromFile(FilePathParam const& in_default_config_path, FilePathParam const& in_persistent_config_path, bool send_notifications)
	{
		if (configuration != nullptr)
			if (RootObjectConfiguration* root_configuration = configuration->GetRootConfiguration())
				return root_configuration->LoadConfigurablePropertiesFromFile(in_default_config_path, in_persistent_config_path, send_notifications);
		return false;
	}

	bool ConfigurableInterface::ReloadDefaultPropertiesFromFile(ReloadConfigurationMode load_mode, bool send_notifications)
	{
		if (configuration != nullptr)
			return configuration->ReloadDefaultPropertiesFromFile(load_mode, send_notifications);
		return false;
	}

}; // namespace chaos