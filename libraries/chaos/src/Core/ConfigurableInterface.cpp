#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ---------------------------------------------------------------------
	// ConfigurableInterfaceBase implementation
	// ---------------------------------------------------------------------

	void ConfigurableInterfaceBase::SetObjectConfiguration(ObjectConfigurationBase* in_configuration)
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

	JSONReadConfiguration ConfigurableInterfaceBase::GetJSONReadConfiguration() const
	{
		if (configuration != nullptr)
			return configuration->GetJSONReadConfiguration();
		return {};
	}

	JSONWriteConfiguration ConfigurableInterfaceBase::GetJSONWriteConfiguration() const
	{
		if (configuration != nullptr)
			return configuration->GetJSONWriteConfiguration();
		return {};
	}

	bool ConfigurableInterfaceBase::GiveChildConfiguration(ConfigurableInterfaceBase* other_configurable, std::string_view key)
	{
		if (configuration != nullptr)
		{
			other_configurable->SetObjectConfiguration(configuration->CreateChildConfiguration(key));
			return true;
		}
		return false;
	}

	bool ConfigurableInterfaceBase::ReloadObjectConfiguration(bool send_notifications)
	{
		if (configuration != nullptr)
			return configuration->Reload(send_notifications);
		return false;
	}

}; // namespace chaos