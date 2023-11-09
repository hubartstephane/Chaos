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
		int value = 0;

		chaos::JSONTools::GetAttribute(GetJSONReadConfiguration(), "toto", value, 666);

		chaos::JSONTools::SetAttribute(GetJSONWriteConfiguration(), "toto", value + 1);

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









class MyApplication : public chaos::Application
{
protected:

	boost::filesystem::path GetJSONWritePath() const
	{
		return GetUserLocalTempPath() / "json_write.json";
	}

	virtual int Main() override
	{
		chaos::RootObjectConfiguration conf;

		conf.LoadConfigurations(boost::filesystem::path{}, GetJSONWritePath());

		chaos::shared_ptr<A> a = new A;
		a->SetObjectConfiguration(conf.CreateChildConfiguration("A"));
		a->Initialize();

		conf.SaveWriteConfiguration(GetJSONWritePath());
		chaos::WinTools::ShowFile(GetJSONWritePath());


		conf.LoadConfigurations(boost::filesystem::path{}, GetJSONWritePath());
			//conf.TriggerConfigurationChanged();


		return 0;
	}
};

int main(int argc, char** argv, char** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}