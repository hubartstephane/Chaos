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
		chaos::JSONTools::GetAttribute(GetJSONReadConfiguration(), "toto", toto, 777);
		chaos::JSONTools::SetAttribute(GetJSONWriteConfiguration(), "toto", toto + 10);

		int iii = 0;
		chaos::JSONTools::GetAttribute(GetJSONWriteConfiguration(), "toto", iii);

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

		ReadValues();

		b = new B;
		GiveChildConfiguration(b.get(), "B/C/D");
		b->Initialize();


	}

	void ReadValues()
	{
		chaos::JSONReadConfiguration read_config = GetJSONReadConfiguration();

		chaos::JSONTools::GetAttribute(read_config, "tutu", tutu, 777);

		tutu = tutu;

		auto s = read_config.read_config->dump(4);

		s = s;
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









class MyApplication : public chaos::Application, public chaos::ConfigurableInterface
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
		chaos::RootObjectConfiguration conf;

		boost::filesystem::path conf_read_path = GetJSONReadPath();
		boost::filesystem::path conf_write_path = GetJSONWritePath();

		chaos::WinTools::ShowFile(conf_read_path);
		chaos::WinTools::ShowFile(conf_write_path);

		conf.SetReadConfigPath(conf_read_path);
		conf.SetPersistentConfigPath(conf_write_path);
		conf.LoadConfiguration();

		chaos::shared_ptr<A> a = new A;
		a->SetObjectConfiguration(conf.CreateChildConfiguration("A"));
		a->Initialize();



		//a->b->ReloadObjectConfiguration(true);
		a->b->ReloadObjectConfiguration(true);

		//conf.LoadConfigurations();

		conf.SavePersistentConfiguration();

//		chaos::WinTools::CopyStringToClipboard(conf.GetJSONReadConfiguration().write_config->dump(2).c_str());


		return 0;
	}
};

int main(int argc, char** argv, char** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}