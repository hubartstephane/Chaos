#include <chaos/chaos.h>

// ---------------------------------------------------------------------

class ConfigurableInterfaceBase;
class ObjectConfiguration;










// ---------------------------------------------------------------------



// ---------------------------------------------------------------------
class B : public chaos::Object, public chaos::ConfigurableInterfaceBase
{
public:

	virtual ~B()
	{
		int i = 0;
		++i;
	}

	void Initialize()
	{
		chaos::JSONTools::GetAttribute(GetJSONReadConfiguration(), "toto", toto, 777);
		chaos::JSONTools::SetAttribute(GetJSONWriteConfiguration(), "toto", toto + 1);

		ReadValues();
	}

	void ReadValues()
	{
		chaos::JSONReadConfiguration read_config = GetJSONReadConfiguration();

		chaos::JSONTools::GetAttribute(read_config, "titi", titi, 777);
		chaos::JSONTools::GetAttribute(read_config, "toto", toto, 777);

		titi = titi;
	}



	virtual void OnConfigurationChanged(chaos::JSONReadConfiguration read_config)
	{
		ReadValues();
	}

	int titi = 0;
	int toto = 0;

};




class A : public chaos::Object, public chaos::ConfigurableInterfaceBase
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

		ReadValues();
	}

	void ReadValues()
	{
		chaos::JSONReadConfiguration read_config = GetJSONReadConfiguration();

		chaos::JSONTools::GetAttribute(read_config, "tutu", tutu, 777);

		tutu = tutu;
	}


	virtual void OnConfigurationChanged(chaos::JSONReadConfiguration read_config)
	{
		ReadValues();
		// b = nullptr; // destroy the child object before it receives the notification of configuration changed
	}

	int tutu = 0;

//protected:

	chaos::shared_ptr<B> b;
};









class MyApplication : public chaos::Application
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
		size_t p = sizeof(nlohmann::json);

		boost::filesystem::path conf_read_path = GetJSONReadPath();
		boost::filesystem::path conf_write_path = GetJSONWritePath();

		chaos::RootObjectConfiguration conf;
		conf.SetReadConfigPath(conf_read_path);
		conf.SetWriteConfigPath(conf_write_path);
		conf.LoadConfigurations();

		chaos::shared_ptr<A> a = new A;
		a->SetObjectConfiguration(conf.CreateChildConfiguration("A"));
		a->Initialize();



		//a->b->ReloadObjectConfiguration(true);
		a->ReloadObjectConfiguration(true);

		//conf.LoadConfigurations();


		conf.SaveWriteConfiguration();

		chaos::WinTools::ShowFile(conf_read_path);
		chaos::WinTools::ShowFile(conf_write_path);

		return 0;
	}
};

int main(int argc, char** argv, char** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}