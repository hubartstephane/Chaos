#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// GLFWHints
	//

	void GLFWHints::ApplyHints()
	{
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debug_context);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_version);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_version);
		glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate); // only usefull in fullscreen mode
		glfwWindowHint(GLFW_OPENGL_PROFILE, opengl_profile);
	}

	bool SaveIntoJSON(nlohmann::json& json, GLFWHints const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "debug_context", src.debug_context);
		JSONTools::SetAttribute(json, "major_version", src.major_version);
		JSONTools::SetAttribute(json, "minor_version", src.minor_version);
		JSONTools::SetAttribute(json, "refresh_rate", src.refresh_rate);
		JSONTools::SetAttribute(json, "opengl_profile", src.opengl_profile);
		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json, GLFWHints& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "debug_context", dst.debug_context);
		JSONTools::GetAttribute(json, "major_version", dst.major_version);
		JSONTools::GetAttribute(json, "minor_version", dst.minor_version);
		JSONTools::GetAttribute(json, "refresh_rate", dst.refresh_rate);
		JSONTools::GetAttribute(json, "opengl_profile", dst.opengl_profile);
		return true;
	}

}; // namespace chaos
