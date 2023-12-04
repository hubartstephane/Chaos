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
		glfwWindowHint(GLFW_SAMPLES, samples);
		glfwWindowHint(GLFW_DOUBLEBUFFER, double_buffer);
		glfwWindowHint(GLFW_DEPTH_BITS, depth_bits);
		glfwWindowHint(GLFW_STENCIL_BITS, stencil_bits);
		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
	}

	bool DoSaveIntoJSON(nlohmann::json * json, GLFWHints const& src)
	{
		if (!PrepareSaveObjectIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "debug_context", src.debug_context);
		JSONTools::SetAttribute(json, "major_version", src.major_version);
		JSONTools::SetAttribute(json, "minor_version", src.minor_version);
		JSONTools::SetAttribute(json, "refresh_rate", src.refresh_rate);
		JSONTools::SetAttribute(json, "opengl_profile", src.opengl_profile);
		JSONTools::SetAttribute(json, "samples", src.samples);
		JSONTools::SetAttribute(json, "double_buffer", src.double_buffer);
		JSONTools::SetAttribute(json, "depth_bits", src.depth_bits);
		JSONTools::SetAttribute(json, "stencil_bits", src.stencil_bits);
		JSONTools::SetAttribute(json, "unlimited_fps", src.unlimited_fps);
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, GLFWHints& dst)
	{
		JSONTools::GetAttribute(config, "debug_context", dst.debug_context);
		JSONTools::GetAttribute(config, "major_version", dst.major_version);
		JSONTools::GetAttribute(config, "minor_version", dst.minor_version);
		JSONTools::GetAttribute(config, "refresh_rate", dst.refresh_rate);
		JSONTools::GetAttribute(config, "opengl_profile", dst.opengl_profile);
		JSONTools::GetAttribute(config, "samples", dst.samples);
		JSONTools::GetAttribute(config, "double_buffer", dst.double_buffer);
		JSONTools::GetAttribute(config, "depth_bits", dst.depth_bits);
		JSONTools::GetAttribute(config, "stencil_bits", dst.stencil_bits);
		JSONTools::GetAttribute(config, "unlimited_fps", dst.unlimited_fps);
		return true;
	}

}; // namespace chaos
