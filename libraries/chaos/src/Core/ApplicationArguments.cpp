#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

namespace chaos
{
	char const* ApplicationArgumentManager::GetOptionString() const
	{
		return options_string.c_str();
	}

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

			std::ostringstream stream;
			stream << desc;
			options_string = std::move(stream.str());
		}
		catch (const boost::program_options::error& ex)
		{
			Log::Output(LogType::Error, false, ex.what());
		}
	}

}; // namespace chaos
