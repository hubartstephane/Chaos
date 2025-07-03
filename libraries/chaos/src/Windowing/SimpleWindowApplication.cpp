#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	SimpleWindowApplication::SimpleWindowApplication(SubClassOf<Window> in_main_window_class, WindowPlacementInfo const& in_main_window_placement_info, WindowCreateParams const& in_main_window_create_params):
		main_window_class(in_main_window_class),
		main_window_placement_info(in_main_window_placement_info),
		main_window_create_params(in_main_window_create_params)
	{
		assert(in_main_window_class.IsValid());
	}

	int SimpleWindowApplication::MainBody()
	{
		// create the main window
		Window* main_window = CreateMainWindow();
		if (main_window == nullptr)
			return -1;

		return WindowApplication::MainBody();
	}

	Window* SimpleWindowApplication::CreateMainWindow()
	{
		WindowCreateParams create_params = main_window_create_params;

		// gives a title to the window (if necessary)
		if (StringTools::IsEmpty(create_params.title))
			if (GetArguments().size() > 0)
				create_params.title = PathTools::PathToName(GetArguments()[0]);
		// create the window
		return DoCreateMainWindow(create_params);
	}

	Window * SimpleWindowApplication::DoCreateMainWindow(WindowCreateParams const & create_params)
	{
		return CreateTypedWindow(main_window_class, main_window_placement_info, create_params, "main_window");
	}

}; // namespace chaos
