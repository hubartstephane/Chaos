#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ---------------------------------------------------------------------
	// ConfigurationUserInterface implementation
	// ---------------------------------------------------------------------

	void ConfigurationUserInterface::SetObjectConfiguration(ObjectConfigurationBase* in_configuration)
	{
		assert(in_configuration == nullptr || in_configuration->configuration_user == nullptr); // cannot set a configuration already in use

		if (configuration.get() != in_configuration)
		{
			// erase previous configuration
			if (configuration != nullptr)
				configuration->configuration_user = nullptr;
			// set new configuration
			configuration = in_configuration;
			if (configuration != nullptr)
				configuration->configuration_user = auto_cast(this);
		}
	}

	JSONReadConfiguration ConfigurationUserInterface::GetJSONReadConfiguration() const
	{
		if (configuration != nullptr)
			return configuration->GetJSONReadConfiguration();
		return {};
	}

	JSONWriteConfiguration ConfigurationUserInterface::GetJSONWriteConfiguration() const
	{
		if (configuration != nullptr)
			return configuration->GetJSONWriteConfiguration();
		return {};
	}

	bool ConfigurationUserInterface::GiveClonedConfiguration(ConfigurationUserInterface* other_configuration_user) const
	{
		if (configuration != nullptr)
		{
			other_configuration_user->SetObjectConfiguration(configuration->CreateClonedDetachedConfiguration());
			return true;
		}
		return false;
	}

	bool ConfigurationUserInterface::GiveChildConfiguration(ConfigurationUserInterface* other_configuration_user, std::string key) const
	{
		if (configuration != nullptr)
		{
			other_configuration_user->SetObjectConfiguration(configuration->CreateChildConfiguration(std::move(key)));
			return true;
		}
		return false;
	}

	bool ConfigurationUserInterface::ReadConfigurableProperties(ReadConfigurablePropertiesContext context, bool recurse)
	{
		if (configuration != nullptr)
			return configuration->ReadConfigurableProperties(context, recurse);
		return false;
	}

	bool ConfigurationUserInterface::StorePersistentProperties(bool recurse) const
	{
		if (configuration != nullptr)
			return configuration->StorePersistentProperties(recurse);
		return false;
	}

	bool ConfigurationUserInterface::OnConfigurationChanged(JSONReadConfiguration config)
	{
		return OnReadConfigurableProperties(config, ReadConfigurablePropertiesContext::HotReload);
	}

	bool ConfigurationUserInterface::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{
		return true;
	}

	bool ConfigurationUserInterface::OnStorePersistentProperties(JSONWriteConfiguration config) const
	{
		return true;
	}

	bool ConfigurationUserInterface::SavePersistentPropertiesToFile(bool store_properties) const
	{
		if (configuration != nullptr)
			if (RootObjectConfiguration* root_configuration = configuration->GetRootConfiguration())
				return root_configuration->SavePersistentPropertiesToFile(store_properties);
		return false;
	}

	bool ConfigurationUserInterface::LoadConfigurablePropertiesFromFile(FilePathParam const& in_default_config_path, FilePathParam const& in_persistent_config_path, bool send_notifications)
	{
		if (configuration != nullptr)
			if (RootObjectConfiguration* root_configuration = configuration->GetRootConfiguration())
				return root_configuration->LoadConfigurablePropertiesFromFile(in_default_config_path, in_persistent_config_path, send_notifications);
		return false;
	}

	bool ConfigurationUserInterface::ReloadDefaultPropertiesFromFile(ReloadConfigurationMode load_mode, bool send_notifications)
	{
		if (configuration != nullptr)
			return configuration->ReloadDefaultPropertiesFromFile(load_mode, send_notifications);
		return false;
	}

}; // namespace chaos