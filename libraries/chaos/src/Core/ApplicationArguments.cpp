#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	char const* ApplicationArgumentManager::GetOptionString() const
	{
		return options_string.c_str();
	}

	void ApplicationArgumentManager::ParseArguments(int argc, char** argv)
	{
		namespace po = boost::program_options;

		try
		{
			po::options_description desc{ "Options" };
			for (ApplicationArgumentBase* argument : arguments)
				argument->RegisterProgramOption(desc);

			po::variables_map vm;
			store(
				po::command_line_parser(argc, argv).
				options(desc).
				allow_unregistered().
				style(po::command_line_style::default_style | po::command_line_style::case_insensitive).
				run(), vm);
			notify(vm);

			std::ostringstream stream;
			stream << desc;
			options_string = std::move(stream.str());
		}
		catch (const po::error& ex)
		{
			Log::Output(LogType::Error, false, ex.what());
		}
	}

}; // namespace chaos
