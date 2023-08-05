#include <chaos/chaos.h>

// ---------------------------------------------------------------------

class ConfigurableInterface;
class ObjectConfiguration;

// ---------------------------------------------------------------------













class ConfigurableInterface
{
	friend class ObjectConfiguration;

public:

	/** destructor */
	virtual ~ConfigurableInterface() = default;

	/** getter on the configuration */
	ObjectConfiguration* GetConfiguration()
	{
		return configuration.get();
	}

protected:

	/** called whenever the configuration has been changed */
	virtual void OnConfigurationChanged(){}

protected:

	/** the configuration */
	chaos::shared_ptr<ObjectConfiguration> configuration;
};


// ---------------------------------------------------------------------

class ObjectConfiguration : public chaos::Object
{
public:

	/** destructor */
	virtual ~ObjectConfiguration();

	/** create a child configuration */
	ObjectConfiguration* CreateChildConfiguration(std::string_view key);
	/** set the configurable for this object */
	void SetConfigurable(chaos::Object* in_configurable);
	/** call this whenever the configuration is being changed */
	void OnConfigurationChanged();

protected:

	void TriggerChangeNotification();

	void UpdateFromParent();

	void PropagateUpdates();

	void PropagateNotifications();

	/** called whenever a child is being destroyed */
	void OnChildConfigDestroyed(ObjectConfiguration* child);

protected:

	/** the json node to read info from */
	nlohmann::json * read_config = nullptr;
	/** the json node to write info into */
	nlohmann::json * write_config = nullptr;

	/** the parent configuration */
	chaos::weak_ptr<ObjectConfiguration> parent_configuration;
	/** the path from parent to this configuration */
	std::string key;
	/** the children configurations */
	std::vector<chaos::weak_ptr<ObjectConfiguration>> child_configurations;
	/** the configurable object owning this */
	chaos::weak_ptr<chaos::Object> configurable_object;

	/** whether the configuration is being updated */
	bool update_in_progress = false;
		/** whether the configuration is being updated */
	bool pending_child_destroy = false;
};

// ---------------------------------------------------------------------

ObjectConfiguration::~ObjectConfiguration()
{
	if (parent_configuration != nullptr)
		parent_configuration->OnChildConfigDestroyed(this);
}

void ObjectConfiguration::OnChildConfigDestroyed(ObjectConfiguration* child)
{
	assert(child != nullptr);
	assert(child->parent_configuration == this);
	if (update_in_progress)
		pending_child_destroy = true;
	else
	{
		//child_configurations.

	}

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

void ObjectConfiguration::SetConfigurable(chaos::Object* in_configurable_object)
{
	assert(configurable_object == nullptr); // cannot be changed after set
	assert(in_configurable_object != nullptr);

	if (ConfigurableInterface* configurable = auto_cast(in_configurable_object))
	{
		configurable_object = in_configurable_object;
		if (configurable != nullptr)
			configurable->configuration = this;
	}
}

void ObjectConfiguration::OnConfigurationChanged()
{
	assert(parent_configuration == nullptr); // should only be called for top level configuration
	PropagateUpdates();
	PropagateNotifications();
}


void ObjectConfiguration::TriggerChangeNotification()
{
	if (ConfigurableInterface * configurable = auto_cast(configurable_object.get()))
		configurable->OnConfigurationChanged();
}

void ObjectConfiguration::UpdateFromParent()
{
	read_config = (parent_configuration != nullptr && parent_configuration->read_config != nullptr) ?
		chaos::JSONTools::GetStructureByPath(*parent_configuration->read_config, key) :
		nullptr;
	write_config = (parent_configuration != nullptr && parent_configuration->write_config != nullptr) ?
		chaos::JSONTools::GetOrCreateStructureByPath(*parent_configuration->write_config, key) :
		nullptr;
}

void ObjectConfiguration::PropagateUpdates()
{
	UpdateFromParent();
	for (chaos::weak_ptr<ObjectConfiguration>& child : child_configurations)
		if (child != nullptr)
			child->PropagateUpdates();
}

void ObjectConfiguration::PropagateNotifications()
{
	TriggerChangeNotification();
	for (chaos::weak_ptr<ObjectConfiguration>& child : child_configurations)
		if (child != nullptr)
			child->PropagateNotifications();
}

// ---------------------------------------------------------------------


class A
{
public:

	A(int ii) :i(ii) {}

	int i = 0;
};

bool operator == (A const& src1, A const& src2)
{
	return (src1.i == src2.i);
}

int main(int argc, char ** argv, char ** env)
{
	bool b1 = (A(3) == A(5));
	bool b2 = (A(3) != A(5));



	return 0;
}