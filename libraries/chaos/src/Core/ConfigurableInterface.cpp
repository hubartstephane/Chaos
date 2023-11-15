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

	bool ConfigurableInterface::GiveChildConfiguration(ConfigurableInterface* other_configurable, std::string key)
	{
		if (configuration != nullptr)
		{
			other_configurable->SetObjectConfiguration(configuration->CreateChildConfiguration(std::move(key)));
			return true;
		}
		return false;
	}

	bool ConfigurableInterface::ReloadObjectConfiguration(bool send_notifications)
	{
		if (configuration != nullptr)
			return configuration->Reload(send_notifications);
		return false;
	}

	void ConfigurableInterface::OnConfigurationChanged(JSONReadConfiguration read_config)
	{
		ReadConfigurableProperties(ReadConfigurablePropertiesContext::HOT_RELOAD);
	}

	void ConfigurableInterface::ReadConfigurableProperties(ReadConfigurablePropertiesContext context)
	{
		OnReadConfigurableProperties(JSONReadConfiguration(), context);
	}

	void ConfigurableInterface::OnReadConfigurableProperties(JSONReadConfiguration config, ReadConfigurablePropertiesContext context)
	{		
	}

}; // namespace chaos