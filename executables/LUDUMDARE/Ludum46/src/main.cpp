#include <chaos/Chaos.h>
#include "Ludum46Game.h"

using namespace chaos;


template<typename T>
static uintptr_t GetClassUniqueID()
{
	static uintptr_t result = 0;
	return (uintptr_t)&result;
}

class ApplicationArgumentBase : public NamedObject
{
public:

	uintptr_t GetClassID() const { return class_id; }

	virtual void RegisterProgramOption(boost::program_options::options_description & desc) = 0;

protected:

	uintptr_t class_id = 0;
};

template<typename T>
class ApplicationArgument : public ApplicationArgumentBase
{
public:


	ApplicationArgument(char const * argument_name)
	{
		class_id = GetClassUniqueID<T>();

		virtual void RegisterProgramOption(boost::program_options::options_description & desc) override
		{
			desc.add_options()
				(argument_name, boost::program_options::value<T>(&value)->default_value(3.14f), argument_name);
		}
	}

protected:

	T value;
};

class ApplicationArgumentManager : public Singleton<ApplicationArgumentManager>
{
public:

	template<typename T>
	ApplicationArgument<T>* FindArgument(char const* name)
	{
		if (ApplicationArgumentBase* result = ObjectRequest(name).FindObject(arguments))
		{
			if (result->GetClassID() == GetClassUniqueID<T>())
			{
				return (ApplicationArgument<T>*)result;
			}
		}
		return nullptr;
	}

	template<typename T>
	T & RegisterArgument(char const* name)
	{
		if (ApplicationArgument<T>* result = FindArgument<T>(name))
		{
			return *result;
		}

		ApplicationArgument<T>* result = new ApplicationArgument<T>(name);
		assert(result != nullptr);
		result->SetName(name);
		arguments.push_back(result);
		return *result;
	}

protected:

	std::vector<ApplicationArgumentBase*> arguments;

};




#define CHAOS_APPLICATION_ARG(ARGNAME, TYPE) static ApplicationArgument<TYPE> & BOOST_PP_CAT(ARGNAME, __LINE__) = ApplicationArgumentManager::GetInstance()->RegisterArgument<TYPE>(#ARGNAME);


CHAOS_APPLICATION_ARG(TOTO, bool);
CHAOS_APPLICATION_ARG(TOTO, int);

//#include <unistd.h>
#include <boost/program_options.hpp>

void on_age(int age)
{
	std::cout << "On age: " << age << '\n';
}

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	try
	{

		float ppp = 0.0f;

		std::vector<std::string> Lists;

		boost::program_options::options_description desc{ "Options" };
		desc.add_options()
			("help,h", "Help screen");
		desc.add_options()
			("pi", boost::program_options::value<float>(&ppp)->default_value(3.14f), "Pi");
		desc.add_options()
		//	("list", boost::program_options::value<std::vector<std::string>>(&Lists[0])->multitoken() , "list");
			("list", boost::program_options::value<std::vector<std::string>>(&Lists)->multitoken(), "list");
		desc.add_options()
			("age", boost::program_options::value<int>()->notifier(on_age), "Age");

		boost::program_options::variables_map vm;
		store(parse_command_line(argc, argv, desc), vm);
		notify(vm);

		auto x = vm["pi"].as<float>();

		auto y = vm["list"].as<std::vector<std::string>>();

		if (vm.count("help"))
			std::cout << desc << '\n';
		else if (vm.count("age"))
			std::cout << "Age: " << vm["age"].as<int>() << '\n';
		else if (vm.count("pi"))
			std::cout << "Pi: " << vm["pi"].as<float>() << '\n';
	}
	catch (const boost::program_options::error& ex)
	{
		std::cerr << ex.what() << '\n';
	}







	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
