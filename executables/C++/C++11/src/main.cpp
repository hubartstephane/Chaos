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
		chaos::ObjectConfigurationBase* conf = GetConfiguration();

		conf = conf;

	}

	virtual void OnConfigurationChanged()
	{
		int i = 0;
		++i;

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
		chaos::ObjectConfigurationBase* conf = GetConfiguration();

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


class G
{
public:

	G(int a, int b, int c):x(a)
	{
		a = a;
	}

	~G()
	{
		int i = 0;
		++i;
	}

	int x;
};

int main(int argc, char ** argv, char ** env)
{
	chaos::RootObjectConfiguration conf;


	chaos::shared_ptr<A> a = new A;


	a->SetConfiguration(conf.CreateChildConfiguration("A"));
	a->Initialize();

	conf.TriggerConfigurationChanged();

	a = nullptr;

	return 0;
}