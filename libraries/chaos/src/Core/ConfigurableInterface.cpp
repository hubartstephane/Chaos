#include "chaos/ChaosPCH.h"
#pragma once

#include <chaos/chaos.h>

namespace chaos
{
	// ---------------------------------------------------------------------
	// ConfigurableInterface implementation
	// ---------------------------------------------------------------------

	void ConfigurableInterface::SetConfiguration(ObjectConfiguration* in_configuration)
	{
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

	// ---------------------------------------------------------------------
	// ObjectConfiguration implementation
	// ---------------------------------------------------------------------

	ObjectConfiguration::~ObjectConfiguration()
	{
		assert(parent_configuration == nullptr);
	}

	void ObjectConfiguration::SubReference()
	{
		if (parent_configuration == nullptr)
			Object::SubReference(); // the configuration is handled as usual
		else if (--shared_count == 1) // the last reference is the one from the parent. Destroy it 
			RemoveFromParent();
	}

	void ObjectConfiguration::RemoveFromParent()
	{
		assert(parent_configuration != nullptr);

		auto it = std::ranges::find_if(parent_configuration->child_configurations, [this](auto const& p) { return p == this; }); // search this in our parent's children list
		if (it != parent_configuration->child_configurations.end())
			parent_configuration->child_configurations.erase(it);

		parent_configuration = nullptr;
	}

	ObjectConfiguration* ObjectConfiguration::CreateChildConfiguration(std::string_view key)
	{
		if (ObjectConfiguration* result = new ObjectConfiguration)
		{
			result->parent_configuration = this;
			result->key = key;
			result->UpdateFromParent();

			child_configurations.push_back(result);

			return result;
		}
		return nullptr;
	}

	void ObjectConfiguration::OnConfigurationChanged()
	{
		assert(parent_configuration == nullptr); // should only be called for top level configuration
		PropagateUpdates();
		PropagateNotifications();
	}

	void ObjectConfiguration::UpdateFromParent()
	{
		read_config = (parent_configuration != nullptr && parent_configuration->read_config != nullptr) ?
			JSONTools::GetStructure(*parent_configuration->read_config, key) :
			nullptr;
		write_config = (parent_configuration != nullptr && parent_configuration->write_config != nullptr) ?
			JSONTools::GetOrCreateStructure(*parent_configuration->write_config, key) :
			nullptr;
	}

	void ObjectConfiguration::PropagateUpdates()
	{
		// update the configuration
		UpdateFromParent();
		// propagate the update to sub hierarchy
		for (shared_ptr<ObjectConfiguration>& child : child_configurations)
			child->PropagateUpdates();
	}

	void ObjectConfiguration::PropagateNotifications()
	{
		// trigger the change for the configurable
		if (ConfigurableInterface* configurable = auto_cast(configurable_object.get()))
			configurable->OnConfigurationChanged();
		// create a weak copy of the children list. children may be destroyed during this loop
		std::vector<weak_ptr<ObjectConfiguration>> child_copy;
		child_copy.reserve(child_configurations.size());
		for (auto& child : child_configurations)
			child_copy.push_back(child.get());
		// propagate the change to sub hierarchy
		for (auto& child : child_copy)
			if (child != nullptr)
				child->PropagateNotifications();
	}

}; // namespace chaos