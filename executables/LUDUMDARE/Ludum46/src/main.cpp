#include <chaos/Chaos.h>
#include "Ludum46Game.h"

#include <boost/program_options.hpp>
#include <sstream>

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
	friend class ApplicationArgumentManager;

public:

	ApplicationArgument(T in_value = {}) : value(in_value)
	{
		class_id = GetClassUniqueID<T>();
	}

	virtual void RegisterProgramOption(boost::program_options::options_description& desc) override
	{
		if constexpr (meta::is_vector_type_v<T>)
		{
			// zero_tokens
			//   accepts empty lists as
			//     --PARAM
			// composing
			//    --PARAM a b c --PARAM d e
			//   is equivalent to
			//    --PARAM a b c d e
			desc.add_options()
				(GetName(), boost::program_options::value<T>(&value)->multitoken()->zero_tokens()->composing(), GetName());
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			// implicit value:
			//     --PARAM
			//   is equivalent to
			//     --PARAM true
			desc.add_options()
				(GetName(), boost::program_options::value<T>(&value)->implicit_value(true), GetName());
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



void func(int X, char const* n, char const* a = nullptr)
{
	n = n;
}

#define FUNC(X, ...) func(X, __VA_ARGS__, __FUNCTION__)



#define CHAOS_APPLICATION_ARG(ARGNAME, TYPE, ...) static TYPE const & ARGNAME = ApplicationArgumentManager::GetInstance()->RegisterArgument<TYPE>(#ARGNAME, __VA_ARGS__);

CHAOS_APPLICATION_ARG(_INT, int, 44);
CHAOS_APPLICATION_ARG(_VECTOR, std::vector<std::string>);
CHAOS_APPLICATION_ARG(_BOOL, bool, false);



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

	ApplicationArgumentManager::GetInstance()->ParseArguments(argc, argv);

	auto X = _INT; // int
	auto Y = _VECTOR; // vector
	auto Z = _BOOL; // bool

	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
