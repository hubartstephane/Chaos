#include <chaos/Chaos.h>

namespace chaos
{
	void ApplicationArgumentManager::ParseArguments(int argc, char** argv)
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

}; // namespace chaos
