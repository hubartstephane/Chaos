#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// ---------------------------------------------------------------------
	// ObjectConfigurationBase implementation
	// ---------------------------------------------------------------------

	ChildObjectConfiguration* ObjectConfigurationBase::CreateChildConfiguration(std::string path)
	{
		if (ChildObjectConfiguration* result = new ChildObjectConfiguration)
		{
			child_configurations.push_back(result);

			result->parent_configuration = this;
			result->path = std::move(path);
			result->UpdateFromParent();

			return result;
		}
		return nullptr;
	}

	void ObjectConfigurationBase::PropagateNotifications()
	{
		// trigger the change for the configurable
		if (ConfigurableInterface* configurable = GetConfigurable())
			configurable->OnConfigurationChanged(GetJSONReadConfiguration());
		// create a weak copy of the children list. children may be destroyed during this loop
		std::vector<weak_ptr<ChildObjectConfiguration>> child_copy;
		child_copy.reserve(child_configurations.size());
		for (auto& child : child_configurations)
			child_copy.push_back(child.get());
		// propagate the change to sub hierarchy
		for (auto& child : child_copy)
			if (child != nullptr)
				child->PropagateNotifications();
	}

	JSONReadConfiguration ObjectConfigurationBase::GetJSONReadConfiguration() const
	{
		JSONReadConfiguration result;
		result.default_config = default_config;
		result.persistent_config = persistent_config;
		return result;
	}

	JSONWriteConfiguration ObjectConfigurationBase::GetJSONWriteConfiguration() const
	{
		JSONWriteConfiguration result;
		result.default_config = default_config;
		result.persistent_config = persistent_config;
		return result;
	}

	void ObjectConfigurationBase::CompleteUpdateOperation(bool send_notifications)
	{
		// propagate the update to sub hierarchy
		for (shared_ptr<ChildObjectConfiguration>& child : child_configurations)
			child->PropagateUpdates();
		// send notifications
		if (send_notifications)
			PropagateNotifications();
	}

	RootObjectConfiguration* ObjectConfigurationBase::GetRootConfiguration()
	{
		if (RootObjectConfiguration* root = auto_cast(this))
			return root;
		if (ChildObjectConfiguration* child = auto_cast(this))
			return child->GetRootConfiguration();
		return nullptr;
	}

	RootObjectConfiguration const* ObjectConfigurationBase::GetRootConfiguration() const
	{
		if (RootObjectConfiguration const* root = auto_cast(this))
			return root;
		if (ChildObjectConfiguration const* child = auto_cast(this))
			return child->GetRootConfiguration();
		return nullptr;
	}

	bool ObjectConfigurationBase::ReloadDefaultPropertiesFromFile(bool partial_reload_only, bool send_notifications)
	{
		// reload whole hiearchy
		if (!partial_reload_only)
			if (RootObjectConfiguration* root = GetRootConfiguration())
				return root->DoLoadConfigurablePropertiesFromFile(true, false, send_notifications); // only reload the DEFAULT part

		// for root
		if (RootObjectConfiguration* root = auto_cast(this))
			return root->DoLoadConfigurablePropertiesFromFile(true, false, send_notifications); // only reload the DEFAULT part

		// for children
		if (ChildObjectConfiguration* child = auto_cast(this))
		{
			// empty current storage
			storage_default_config = {};
			default_config = nullptr;

			// temp storage for the whole hierarchy
			nlohmann::json new_root_storage;

			// recursively go to root and ask for this node content
			if (nlohmann::json const* new_default_config = ReloadHelper(new_root_storage, child->parent_configuration.get(), child->path))
			{
				storage_default_config = *new_default_config; // copy the whole json structure
				default_config = &storage_default_config;
			}

			// update down hierarchy and send notifications (parents and siblings conf are unchanged and receive non notification)
			CompleteUpdateOperation(send_notifications);

			return true;
		}
		return false;
	}

	nlohmann::json const* ObjectConfigurationBase::ReloadHelper(nlohmann::json& new_root_storage, ObjectConfigurationBase* src, std::string_view in_path)
	{
		// root case
		if (RootObjectConfiguration* root = auto_cast(src))
		{
			JSONTools::LoadJSONFile(root->default_config_path, new_root_storage, LoadFileFlag::RECURSIVE | LoadFileFlag::NO_ERROR_TRACE);
			return JSONTools::GetStructureNode(new_root_storage, in_path);
		}
		// child case
		if (ChildObjectConfiguration* child = auto_cast(src))
			if (nlohmann::json const* new_child_default_json = ReloadHelper(new_root_storage, child->parent_configuration.get(), child->path))
				return JSONTools::GetStructureNode(*new_child_default_json, in_path);
		// error
		return nullptr;
	}

	bool ObjectConfigurationBase::ReadConfigurableProperties(ReadConfigurablePropertiesContext context, bool recurse)
	{
		ConfigurableInterface* configurable = GetConfigurable();
		if (configurable == nullptr)
			return false;
		if (!configurable->OnReadConfigurableProperties(GetJSONReadConfiguration(), context))
			return false;
		if (recurse)
			for (shared_ptr<ChildObjectConfiguration>& child : child_configurations)
				if (!child->ReadConfigurableProperties(context, recurse))
					return false;
		return true;
	}

	bool ObjectConfigurationBase::StorePersistentProperties(bool recurse) const
	{
		if (ConfigurableInterface const* configurable = GetConfigurable())
			if (!configurable->OnStorePersistentProperties(GetJSONWriteConfiguration()))
				return false;
		if (recurse)
			for (shared_ptr<ChildObjectConfiguration> const& child : child_configurations)
				if (!child->StorePersistentProperties(recurse))
					return false;
		return true;
	}

	ConfigurableInterface* ObjectConfigurationBase::GetConfigurable()
	{
		return auto_cast(configurable_object.get());
	}

	ConfigurableInterface const* ObjectConfigurationBase::GetConfigurable() const
	{
		return auto_cast(configurable_object.get());
	}

	// ---------------------------------------------------------------------
	// ChildObjectConfiguration implementation
	// ---------------------------------------------------------------------

	ChildObjectConfiguration::~ChildObjectConfiguration()
	{
		assert(parent_configuration == nullptr);
	}

	void ChildObjectConfiguration::SubReference()
	{
		if (parent_configuration == nullptr)
			Object::SubReference(); // the configuration is handled as usual
		else if (--shared_count == 1) // the last reference is the one from the parent. Destroy it 
			RemoveFromParent();
	}

	void ChildObjectConfiguration::RemoveFromParent()
	{
		assert(parent_configuration != nullptr);

		auto it = std::ranges::find_if(parent_configuration->child_configurations, [this](auto const& p) { return p == this; }); // search this in our parent's children list
		if (it != parent_configuration->child_configurations.end())
			parent_configuration->child_configurations.erase(it);

		parent_configuration = nullptr;
	}

	void ChildObjectConfiguration::UpdateFromParent()
	{
		default_config = (parent_configuration != nullptr && parent_configuration->default_config != nullptr) ?
			JSONTools::GetObjectNode(*parent_configuration->default_config, path) :
			nullptr;
		persistent_config = (parent_configuration != nullptr && parent_configuration->persistent_config != nullptr) ?
			JSONTools::GetOrCreateObjectNode(*parent_configuration->persistent_config, path) :
			nullptr;

		storage_default_config = {}; // empty self storage
	}

	void ChildObjectConfiguration::PropagateUpdates()
	{
		// update the configuration
		UpdateFromParent();
		// propagate the update to sub hierarchy
		for (shared_ptr<ChildObjectConfiguration>& child : child_configurations)
			child->PropagateUpdates();
	}

	// ---------------------------------------------------------------------
	// RootObjectConfiguration implementation
	// ---------------------------------------------------------------------

	RootObjectConfiguration::RootObjectConfiguration()
	{
		default_config = &storage_default_config;
		persistent_config = &storage_persistent_config;
	}


	bool RootObjectConfiguration::LoadConfigurablePropertiesFromFile(FilePathParam const& in_default_config_path, FilePathParam const& in_persistent_config_path, bool send_notifications)
	{
		SetDefaultConfigurationPath(in_default_config_path);
		SetPersistentConfigurationPath(in_persistent_config_path);
		return DoLoadConfigurablePropertiesFromFile(true, true, send_notifications);
	}

	bool RootObjectConfiguration::DoLoadConfigurablePropertiesFromFile(bool load_default, bool load_persistent, bool send_notifications)
	{
		bool changed = false;

		// update the default json
		if (load_default)
		{
			if (!default_config_path.empty())
			{
				storage_default_config = nlohmann::json();
				JSONTools::LoadJSONFile(default_config_path, storage_default_config, LoadFileFlag::RECURSIVE | LoadFileFlag::NO_ERROR_TRACE);
				changed = true;
			}
		}
		// update the persistent json
		if (load_persistent)
		{
			if (!persistent_config_path.empty())
			{
				storage_persistent_config = nlohmann::json();
				JSONTools::LoadJSONFile(persistent_config_path, storage_persistent_config, LoadFileFlag::RECURSIVE | LoadFileFlag::NO_ERROR_TRACE);
				changed = true;
			}
		}
		// notify the changes
		if (changed)
			CompleteUpdateOperation(send_notifications);

		return true;
	}

	bool RootObjectConfiguration::SavePersistentPropertiesToFile(bool store_properties) const
	{
		if (persistent_config_path.empty())
			return false;
		if (store_properties)
			if (!StorePersistentProperties(true))
				return false;
		return JSONTools::SaveJSONToFile(storage_persistent_config, persistent_config_path);
	}
	
	void RootObjectConfiguration::SetDefaultConfigurationPath(FilePathParam const& in_default_config_path)
	{
		default_config_path = in_default_config_path.GetResolvedPath();
	}
	
	void RootObjectConfiguration::SetPersistentConfigurationPath(FilePathParam const& in_persistent_config_path)
	{
		persistent_config_path = in_persistent_config_path.GetResolvedPath();
	}

}; // namespace chaos