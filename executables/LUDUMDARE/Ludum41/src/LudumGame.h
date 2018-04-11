#pragma once

#include <chaos/ReferencedObject.h>

class LudumGame : public chaos::ReferencedObject
{
public:

	/** the tick method */
	void Tick(double delta_time);
	/** whenever a key event is received */
	bool OnKeyEvent(int key, int action);


	/** initialization from the config file */
	bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);

protected:

	/** the window in GLFW library */
	GLFWwindow * glfw_window = nullptr;
};