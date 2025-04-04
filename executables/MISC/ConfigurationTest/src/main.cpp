#include <chaos/chaos.h>

// ---------------------------------------------------------------------

class ConfigurationUserInterface;
class ObjectConfiguration;










// ---------------------------------------------------------------------



// ---------------------------------------------------------------------
class B : public chaos::Object, public chaos::ConfigurationUserInterface
{
public:

	virtual ~B()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		ReadConfigurableProperties(chaos::ReadConfigurablePropertiesContext::Initialization, false);
	}

	virtual bool OnReadConfigurableProperties(chaos::JSONReadConfiguration config, chaos::ReadConfigurablePropertiesContext context) override
	{
		chaos::JSONTools::GetAttribute(config, "titi", titi, 777);
		chaos::JSONTools::GetAttribute(config, "toto", toto, 777);
		return true;
	}

	bool OnStorePersistentProperties(chaos::JSONWriteConfiguration config) const override
	{
		chaos::JSONTools::SetAttribute(config, "toto", toto + 10);
		return true;
	}

public:

	int titi = 0;
	int toto = 0;

};




class A : public chaos::Object, public chaos::ConfigurationUserInterface
{
public:

	virtual ~A()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		ReadConfigurableProperties(chaos::ReadConfigurablePropertiesContext::Initialization, false);

		b = new B;
		GiveChildConfiguration(b.get(), "B/C/D");
		b->Initialize();
	}

	virtual bool OnReadConfigurableProperties(chaos::JSONReadConfiguration config, chaos::ReadConfigurablePropertiesContext context) override
	{
		chaos::JSONTools::GetAttribute(config, "tutu", tutu, 777);
		return true;
	}

	bool OnStorePersistentProperties(chaos::JSONWriteConfiguration config) const override
	{
		return true;
	}

public:

	int tutu = 0;

	chaos::shared_ptr<B> b;
};


class MyApplication : public chaos::Application, public chaos::ConfigurationUserInterface
{
protected:

	boost::filesystem::path GetJSONWritePath() const
	{
		return GetUserLocalTempPath() / "json_write.json";
	}

	boost::filesystem::path GetJSONReadPath() const
	{
		return GetResourcesPath() / "json_read.json";
	}

	virtual int Main() override
	{
		boost::filesystem::path conf_read_path = GetJSONReadPath();
		boost::filesystem::path conf_write_path = GetJSONWritePath();

		chaos::WinTools::ShowFile(conf_read_path);
		chaos::WinTools::ShowFile(conf_write_path);

		chaos::RootObjectConfiguration conf;
		conf.LoadConfigurablePropertiesFromFile(conf_read_path, conf_write_path, false);

		chaos::shared_ptr<A> a = new A;
		a->SetObjectConfiguration(conf.CreateChildConfiguration("A"));
		a->Initialize();

		a->b->ReloadDefaultPropertiesFromFile(true, true);

		conf.SavePersistentPropertiesToFile(true);

		return 0;
	}
};

int main(int argc, char** argv, char** env)
{
	//nlohmann::json j;

	//j.get<MyApplication>();



	chaos::ObjectPool<int> p;
	p.Allocate(3);
	p.Allocate(4);
	p.Allocate(6);

	auto const& pp = p;

	p.ForEachObject([](int  * o)
	{
		int i = 0;
		++i;
	});

	return 0;



	return chaos::RunApplication<MyApplication>(argc, argv, env);
}