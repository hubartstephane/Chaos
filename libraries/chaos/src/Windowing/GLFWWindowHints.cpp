#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	/**
	* GLFWWindowHints
	*/

	void GLFWWindowHints::ApplyHints() const
	{
		glfwWindowHint(GLFW_RESIZABLE, resizable);
		glfwWindowHint(GLFW_VISIBLE, start_visible);
		glfwWindowHint(GLFW_DECORATED, decorated);
		glfwWindowHint(GLFW_FLOATING, toplevel);
		glfwWindowHint(GLFW_SAMPLES, samples);
		glfwWindowHint(GLFW_DOUBLEBUFFER, double_buffer);
		glfwWindowHint(GLFW_DEPTH_BITS, depth_bits);
		glfwWindowHint(GLFW_STENCIL_BITS, stencil_bits);
		glfwWindowHint(GLFW_RED_BITS, red_bits);
		glfwWindowHint(GLFW_GREEN_BITS, green_bits);
		glfwWindowHint(GLFW_BLUE_BITS, blue_bits);
		glfwWindowHint(GLFW_ALPHA_BITS, alpha_bits);
		glfwWindowHint(GLFW_FOCUSED, focused);
	}

	bool SaveIntoJSON(nlohmann::json& json, GLFWWindowHints const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
		JSONTools::SetAttribute(json, "resizable", src.resizable);
		JSONTools::SetAttribute(json, "start_visible", src.start_visible);
		JSONTools::SetAttribute(json, "decorated", src.decorated);
		JSONTools::SetAttribute(json, "toplevel", src.toplevel);
		JSONTools::SetAttribute(json, "focused", src.focused);
		JSONTools::SetAttribute(json, "samples", src.samples);
		JSONTools::SetAttribute(json, "double_buffer", src.double_buffer);
		JSONTools::SetAttribute(json, "depth_bits", src.depth_bits);
		JSONTools::SetAttribute(json, "stencil_bits", src.stencil_bits);
		JSONTools::SetAttribute(json, "red_bits", src.red_bits);
		JSONTools::SetAttribute(json, "green_bits", src.green_bits);
		JSONTools::SetAttribute(json, "blue_bits", src.blue_bits);
		JSONTools::SetAttribute(json, "alpha_bits", src.alpha_bits);

		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json, GLFWWindowHints& dst)
	{
		if (!json.is_object())
			return false;
		JSONTools::GetAttribute(json, "resizable", dst.resizable);
		JSONTools::GetAttribute(json, "start_visible", dst.start_visible);
		JSONTools::GetAttribute(json, "decorated", dst.decorated);
		JSONTools::GetAttribute(json, "toplevel", dst.toplevel);
		JSONTools::GetAttribute(json, "focused", dst.focused);
		JSONTools::GetAttribute(json, "samples", dst.samples);
		JSONTools::GetAttribute(json, "double_buffer", dst.double_buffer);
		JSONTools::GetAttribute(json, "depth_bits", dst.depth_bits);
		JSONTools::GetAttribute(json, "stencil_bits", dst.stencil_bits);
		JSONTools::GetAttribute(json, "red_bits", dst.red_bits);
		JSONTools::GetAttribute(json, "green_bits", dst.green_bits);
		JSONTools::GetAttribute(json, "blue_bits", dst.blue_bits);
		JSONTools::GetAttribute(json, "alpha_bits", dst.alpha_bits);
		return true;
	}

}; // namespace chaos
