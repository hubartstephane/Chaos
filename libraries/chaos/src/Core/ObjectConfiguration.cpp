#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// ---------------------------------------------------------------------
	// ObjectConfigurationBase implementation
	// ---------------------------------------------------------------------

	ObjectConfigurationBase* ObjectConfigurationBase::CreateClonedConfiguration()
	{
		return nullptr;
	}

	ChildObjectConfiguration* ObjectConfigurationBase::CreateChildConfiguration(std::string path)
	{
		if (ChildObjectConfiguration* result = new ChildObjectConfiguration)
		{
			child_configurations.push_back(result);

			result->parent_configuration = this;
			result->path = std::move(path);
			result->UpdateInternalConfigsFromParent();

			return result;
		}
		return nullptr;
	}

	void ObjectConfigurationBase::PropagateNotificationToUsers()
	{
		// trigger the change for the configurable
		if (ConfigurableInterface* configurable = GetConfigurable())
			configurable->OnConfigurationChanged(GetJSONReadConfiguration());
		// propagate the change to sub hierarchy
		for (shared_ptr<ChildObjectConfiguration>& child : child_configurations)
			child->PropagateNotificationToUsers();
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
			child->PropagateInternalConfigsUpdates();
		// send notifications
		if (send_notifications)
			PropagateNotificationToUsers();
	}

	RootObjectConfiguration* ObjectConfigurationBase::GetRootConfiguration()
	{
		return nullptr;
	}

	RootObjectConfiguration const* ObjectConfigurationBase::GetRootConfiguration() const
	{
		return nullptr;
	}

	bool ObjectConfigurationBase::ReloadDefaultPropertiesFromFile(ReloadConfigurationMode load_mode, bool send_notifications)
	{
		return false;
	}

	nlohmann::json const* ObjectConfigurationBase::ReloadHelper(nlohmann::json& new_root_storage, std::string_view in_path)
	{
		return nullptr;
	}

	bool ObjectConfigurationBase::ReadConfigurableProperties(ReadConfigurablePropertiesContext context, bool recurse)
	{
		if (ConfigurableInterface * configurable = GetConfigurable())
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

	RootObjectConfiguration* ChildObjectConfiguration::GetRootConfiguration()
	{
		if (parent_configuration != nullptr)
			return parent_configuration->GetRootConfiguration();
		return nullptr;
	}

	RootObjectConfiguration const* ChildObjectConfiguration::GetRootConfiguration() const
	{
		if (parent_configuration != nullptr)
			return parent_configuration->GetRootConfiguration();
		return nullptr;
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

	void ChildObjectConfiguration::UpdateInternalConfigsFromParent()
	{
		default_config = (parent_configuration != nullptr) ?
			JSONTools::GetAttributeObjectNode(parent_configuration->default_config, path) :
			nullptr;
		persistent_config = (parent_configuration != nullptr) ?
			JSONTools::GetOrCreateAttributeObjectNode(parent_configuration->persistent_config, path) :
			nullptr;

		storage_default_config = {}; // empty self storage
	}

	void ChildObjectConfiguration::PropagateInternalConfigsUpdates()
	{
		// update the configuration
		UpdateInternalConfigsFromParent();
		// propagate the update to sub hierarchy
		for (shared_ptr<ChildObjectConfiguration>& child : child_configurations)
			child->PropagateInternalConfigsUpdates();
	}

	ObjectConfigurationBase* ChildObjectConfiguration::CreateClonedConfiguration()
	{
		if (ChildObjectConfiguration* result = new ChildObjectConfiguration)
		{
			child_configurations.push_back(result);

			result->parent_configuration = parent_configuration;
			result->path = path;
			result->UpdateInternalConfigsFromParent();

			return result;
		}
		return nullptr;
	}

	nlohmann::json const* ChildObjectConfiguration::ReloadHelper(nlohmann::json& new_root_storage, std::string_view in_path)
	{
		if (nlohmann::json const* new_child_default_json = parent_configuration->ReloadHelper(new_root_storage, path))
			return JSONTools::GetAttributeStructureNode(new_child_default_json, in_path);
		return nullptr;
	}

	bool ChildObjectConfiguration::ReloadDefaultPropertiesFromFile(ReloadConfigurationMode load_mode, bool send_notifications)
	{
		// reload whole hiearchy ?
		if (load_mode == ReloadConfigurationMode::FullLoad)
			if (RootObjectConfiguration* root = GetRootConfiguration())
				return root->DoLoadConfigurablePropertiesFromFile(true, false, send_notifications); // only reload the DEFAULT part (ignore PERSISTENT)

		// at this point, we want just to update our JSON node content and don't modify any accestor node
		// we have to reload the whole JSON file, but dont set the result to parent. we'll trunk this hierarchy starting at our node, and propagate the info to our children
		// we are now in a state detached from the root json hierarchy
		// we are storing our own hierachy into 'storage_default_config'

		// empty current storage
		storage_default_config = {};
		default_config = nullptr;

		// temp storage for the whole hierarchy
		nlohmann::json new_root_storage;

		// recursively go to root and ask for this node content
		if (nlohmann::json const* new_default_config = parent_configuration->ReloadHelper(new_root_storage, path))
		{
			storage_default_config = *new_default_config; // copy the whole json structure
			default_config = &storage_default_config;
		}

		// update down hierarchy and send notifications (parents and siblings conf are unchanged and receive no notification)
		CompleteUpdateOperation(send_notifications);

		return true;
	}

	// ---------------------------------------------------------------------
	// RootObjectConfiguration implementation
	// ---------------------------------------------------------------------

	RootObjectConfiguration::RootObjectConfiguration()
	{
		default_config = &storage_default_config;
		persistent_config = &storage_persistent_config;
	}

	RootObjectConfiguration* RootObjectConfiguration::GetRootConfiguration()
	{
		return this;
	}

	RootObjectConfiguration const* RootObjectConfiguration::GetRootConfiguration() const
	{
		return this;
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
		return JSONTools::SaveJSONToFile(&storage_persistent_config, persistent_config_path);
	}

	void RootObjectConfiguration::SetDefaultConfigurationPath(FilePathParam const& in_default_config_path)
	{
		default_config_path = in_default_config_path.GetResolvedPath();
	}

	void RootObjectConfiguration::SetPersistentConfigurationPath(FilePathParam const& in_persistent_config_path)
	{
		persistent_config_path = in_persistent_config_path.GetResolvedPath();
	}

	ObjectConfigurationBase * RootObjectConfiguration::CreateClonedConfiguration()
	{
		if (RootObjectConfiguration* result = new RootObjectConfiguration)
		{
			result->LoadConfigurablePropertiesFromFile(default_config_path, persistent_config_path, false); // don't send notifications
			return result;
		}
		return nullptr;
	}

	bool RootObjectConfiguration::ReloadDefaultPropertiesFromFile(ReloadConfigurationMode load_mode, bool send_notifications)
	{
		return DoLoadConfigurablePropertiesFromFile(true, false, send_notifications); // only reload the DEFAULT part
	}

	nlohmann::json const* RootObjectConfiguration::ReloadHelper(nlohmann::json& new_root_storage, std::string_view in_path)
	{
		JSONTools::LoadJSONFile(default_config_path, new_root_storage, LoadFileFlag::RECURSIVE | LoadFileFlag::NO_ERROR_TRACE);
		return JSONTools::GetAttributeStructureNode(&new_root_storage, in_path);
	}

}; // namespace chaos