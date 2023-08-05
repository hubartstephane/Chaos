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

	void SetConfiguration(ObjectConfiguration* in_configuration);

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
	friend class ConfigurableInterface;

public:

	/** destructor */
	virtual ~ObjectConfiguration();

	/** override */
	virtual void SubReference() override;

	/** create a child configuration */
	ObjectConfiguration* CreateChildConfiguration(std::string_view key);
	/** call this whenever the configuration is being changed */
	void OnConfigurationChanged();

protected:

	void UpdateFromParent();

	void PropagateUpdates();

	void PropagateNotifications();

	void RemoveFromParent();




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
	std::vector<chaos::shared_ptr<ObjectConfiguration>> child_configurations;
	/** the configurable object owning this */
	chaos::weak_ptr<chaos::Object> configurable_object;
};
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
			configuration->configurable_object = chaos::auto_cast(this);
	}
}

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
		chaos::JSONTools::GetStructureByPath(*parent_configuration->read_config, key) :
		nullptr;
	write_config = (parent_configuration != nullptr && parent_configuration->write_config != nullptr) ?
		chaos::JSONTools::GetOrCreateStructureByPath(*parent_configuration->write_config, key) :
		nullptr;
}

void ObjectConfiguration::PropagateUpdates()
{
	// update the configuration
	UpdateFromParent();
	// propagate the update to sub hierarchy
	for (chaos::shared_ptr<ObjectConfiguration>& child : child_configurations)
		child->PropagateUpdates();
}

void ObjectConfiguration::PropagateNotifications()
{
	// trigger the change for the configurable
	if (ConfigurableInterface* configurable = auto_cast(configurable_object.get()))
		configurable->OnConfigurationChanged();
	// create a weak copy of the children list. children may be destroyed during this loop
	std::vector<chaos::weak_ptr<ObjectConfiguration>> child_copy;
	child_copy.reserve(child_configurations.size());
	for (auto& child : child_configurations)
		child_copy.push_back(child.get());
	// propagate the change to sub hierarchy
	for (auto & child : child_copy)
		if (child != nullptr)
			child->PropagateNotifications();
}

// ---------------------------------------------------------------------
class B : public chaos::Object, public ConfigurableInterface
{
public:

	virtual ~B()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		ObjectConfiguration* conf = GetConfiguration();

		conf = conf;

	}

	virtual void OnConfigurationChanged()
	{
		int i = 0;
		++i;

	}

};




class A : public chaos::Object, public ConfigurableInterface
{
public:

	virtual ~A()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		ObjectConfiguration* conf = GetConfiguration();

		conf = conf;

		b = new B;
		b->SetConfiguration(conf->CreateChildConfiguration("B"));
	}

	virtual void OnConfigurationChanged()
	{
		int i = 0;
		++i;

		b = nullptr;
	}

	chaos::shared_ptr<B> b;

};




int main(int argc, char ** argv, char ** env)
{
	ObjectConfiguration conf;


	chaos::shared_ptr<A> a = new A;


	a->SetConfiguration(conf.CreateChildConfiguration("A"));
	a->Initialize();

	conf.OnConfigurationChanged();

	a = nullptr;

	return 0;
}