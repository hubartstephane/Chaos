#include <chaos/chaos.h>

// ---------------------------------------------------------------------

class ConfigurableInterface;
class ObjectConfiguration;










// ---------------------------------------------------------------------



// ---------------------------------------------------------------------
class B : public chaos::Object, public chaos::ConfigurableInterface
{
public:

	virtual ~B()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		chaos::ObjectConfigurationBase* conf = GetObjectConfiguration();

		int i = 0;

		bool b2 = chaos::JSONTools::GetAttributeByIndex(GetJSONReadConfiguration(), 3, i);

		conf = conf;

	}

	virtual void OnConfigurationChanged()
	{
		int i = 0;
		bool b = chaos::JSONTools::GetAttributeByIndex(GetJSONReadConfiguration(), 3, i);

		b = b;

	}

};




class A : public chaos::Object, public chaos::ConfigurableInterface
{
public:

	virtual ~A()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		chaos::ObjectConfigurationBase* conf = GetObjectConfiguration();

		b = new B;
		b->SetObjectConfiguration(conf->CreateChildConfiguration("B"));
		b->Initialize();
	}

	virtual void OnConfigurationChanged()
	{
		b = nullptr; // destroy the child object before it receives the notification of configuration changed
	}

protected:

	chaos::shared_ptr<B> b;
};

int main(int argc, char ** argv, char ** env)
{
	chaos::RootObjectConfiguration conf;

	chaos::shared_ptr<A> a = new A;
	a->SetObjectConfiguration(conf.CreateChildConfiguration("A"));
	a->Initialize();

	conf.TriggerConfigurationChanged();

	a = nullptr;

	return 0;
}