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
bool constexpr is_vector_type_v = false;

template<typename T>
bool constexpr is_vector_type_v<std::vector<T>> = true;

template<typename T>
class ApplicationArgument : public ApplicationArgumentBase
{
	friend class ApplicationArgumentManager;

public:

	ApplicationArgument()
	{
		class_id = GetClassUniqueID<T>();
	}

	ApplicationArgument(T in_value): value(in_value)
	{
		class_id = GetClassUniqueID<T>();
	}

	virtual void RegisterProgramOption(boost::program_options::options_description& desc) override
	{
		if constexpr (is_vector_type_v<T>)
		{
			desc.add_options()
				(GetName(), boost::program_options::value<T>(&value)->multitoken(), GetName());
		}
		else
		{
			desc.add_options()
				(GetName(), boost::program_options::value<T>(&value), GetName());
		}
	}

protected:

	T value = {};
};

class ApplicationArgumentManager : public Singleton<ApplicationArgumentManager>
{
public:

	template<typename T>
	T const& RegisterArgument(char const* name, T default_value = {})
	{
		if (ApplicationArgument<T>* result = FindArgument<T>(name))
		{
			return result->value;
		}

		ApplicationArgument<T>* result = new ApplicationArgument<T>(default_value);
		assert(result != nullptr);
		result->SetName(name);
		arguments.push_back(result);
		return result->value;
	}

	void ParseArguments(int argc, char **argv)
	{
		try
		{
			boost::program_options::options_description desc{ "Options" };
			for (ApplicationArgumentBase* argument : arguments)
				argument->RegisterProgramOption(desc);

			boost::program_options::variables_map vm;
			store(boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
			notify(vm);
		}
		catch (const boost::program_options::error& ex)
		{
			std::cerr << ex.what() << '\n';
		}
	}

protected:

	template<typename T>
	ApplicationArgument<T>* FindArgument(char const* name)
	{
		if (ApplicationArgumentBase* result = ObjectRequest(name).FindObject(arguments))
			if (result->GetClassID() == GetClassUniqueID<T>())
				return (ApplicationArgument<T>*)result;
		return nullptr;
	}

protected:

	std::vector<ApplicationArgumentBase*> arguments;

};




#define CHAOS_APPLICATION_ARG(ARGNAME, TYPE, ...) static TYPE const & ARGNAME = ApplicationArgumentManager::GetInstance()->RegisterArgument<TYPE>(#ARGNAME, __VA_ARGS__);


//CHAOS_APPLICATION_ARG(TOTO, int, 12345);
//CHAOS_APPLICATION_ARG(TITI, std::vector<std::string>, {"ght", "ytu"});

CHAOS_APPLICATION_ARG(TOTO, int);
CHAOS_APPLICATION_ARG(TITI, std::vector<std::string>);

#include <boost/program_options.hpp>

void on_age(int age)
{
	std::cout << "On age: " << age << '\n';
}

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	auto a = is_vector_type_v<int>;

	auto c = is_vector_type_v<std::vector<std::string>>;

	ApplicationArgumentManager::GetInstance()->ParseArguments(argc, argv);

	auto X = TOTO;

	auto Y = TITI;








	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
