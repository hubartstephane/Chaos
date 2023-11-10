#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// ---------------------------------------------------------------------
	// ObjectConfigurationBase implementation
	// ---------------------------------------------------------------------

	ChildObjectConfiguration* ObjectConfigurationBase::CreateChildConfiguration(std::string_view key)
	{
		if (ChildObjectConfiguration* result = new ChildObjectConfiguration)
		{
			child_configurations.push_back(result);

			result->parent_configuration = this;
			result->key = key;
			result->UpdateFromParent();

			return result;
		}
		return nullptr;
	}

	void ObjectConfigurationBase::PropagateNotifications()
	{
		// trigger the change for the configurable
		if (ConfigurableInterfaceBase* configurable = auto_cast(configurable_object.get()))
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
		result.read_config = read_config;
		result.write_config = write_config;
		return result;
	}

	JSONWriteConfiguration ObjectConfigurationBase::GetJSONWriteConfiguration() const
	{
		JSONWriteConfiguration result;
		result.read_config = read_config;
		result.write_config = write_config;
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

	bool ObjectConfigurationBase::Reload(bool send_notifications)
	{
		// for root
		if (RootObjectConfiguration* root = auto_cast(this))
			return root->LoadConfigurations(true, false, send_notifications); // only reload the READ part

		// for children
		if (ChildObjectConfiguration* child = auto_cast(this))
		{
			// empty current storage
			storage_read_config = {};
			read_config = nullptr;

			// temp storage for the whole hierarchy
			nlohmann::json new_root_storage;

			// recursively go to root and ask for this node content
			if (nlohmann::json const* new_read_config = ReloadHelper(new_root_storage, child->parent_configuration.get(), child->key))
			{
				storage_read_config = *new_read_config; // copy the whole json structure
				read_config = &storage_read_config;
			}

			// update down hierarchy and send notifications (parents and siblings conf are unchanged and receive non notification)
			CompleteUpdateOperation(send_notifications);

			return true;
		}
		return false;
	}

	nlohmann::json const* ObjectConfigurationBase::ReloadHelper(nlohmann::json& new_root_storage, ObjectConfigurationBase* src, std::string_view in_key)
	{
		// root case
		if (RootObjectConfiguration* root = auto_cast(src))
		{
			JSONTools::LoadJSONFile(root->write_config_path, new_root_storage, LoadFileFlag::RECURSIVE);
			return JSONTools::GetStructure(new_root_storage, in_key);
		}
		// child case
		if (ChildObjectConfiguration* child = auto_cast(src))
			if (nlohmann::json const* new_child_read_json = ReloadHelper(new_root_storage, child->parent_configuration.get(), child->key))
				return JSONTools::GetStructure(*new_child_read_json, in_key);
		// error
		return nullptr;
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
		read_config = (parent_configuration != nullptr && parent_configuration->read_config != nullptr) ?
			JSONTools::GetStructure(*parent_configuration->read_config, key) :
			nullptr;
		write_config = (parent_configuration != nullptr && parent_configuration->write_config != nullptr) ?
			JSONTools::GetOrCreateStructure(*parent_configuration->write_config, key) :
			nullptr;

		storage_read_config = {}; // empty self storage
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
		read_config = &storage_read_config;
		write_config = &storage_write_config;
	}

	bool RootObjectConfiguration::LoadConfigurations(bool load_read, bool load_write, bool send_notifications)
	{
		bool changed = false;

		// update the read json
		if (load_read)
		{
			if (!read_config_path.empty())
			{
				storage_read_config = nlohmann::json();
				JSONTools::LoadJSONFile(read_config_path, storage_read_config, LoadFileFlag::RECURSIVE);
				changed = true;
			}
		}
		// update the write json
		if (load_write)
		{
			if (!write_config_path.empty())
			{
				storage_write_config = nlohmann::json();
				JSONTools::LoadJSONFile(write_config_path, storage_write_config, LoadFileFlag::RECURSIVE);
				changed = true;
			}
		}
		// notify the changes
		if (changed)
			CompleteUpdateOperation(send_notifications);

		return true;
	}

	bool RootObjectConfiguration::SaveWriteConfiguration()
	{
		if (write_config_path.empty())
			return false;
		return JSONTools::SaveJSONToFile(storage_write_config, write_config_path);
	}
	
	void RootObjectConfiguration::SetReadConfigPath(FilePathParam const& in_read_config_path)
	{
		read_config_path = in_read_config_path.GetResolvedPath();
	}
	
	void RootObjectConfiguration::SetWriteConfigPath(FilePathParam const& in_write_config_path)
	{
		write_config_path = in_write_config_path.GetResolvedPath();
	}

}; // namespace chaos