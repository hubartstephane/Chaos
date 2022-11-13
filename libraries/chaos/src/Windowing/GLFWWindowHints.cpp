#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	/**
	* GLFWWindowHints
	*/

	void GLFWWindowHints::ApplyHints() const
	{
		glfwWindowHint(GLFW_RED_BITS, red_bits);
		glfwWindowHint(GLFW_GREEN_BITS, green_bits);
		glfwWindowHint(GLFW_BLUE_BITS, blue_bits);
		glfwWindowHint(GLFW_ALPHA_BITS, alpha_bits);
	}

	bool SaveIntoJSON(nlohmann::json& json, GLFWWindowHints const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();
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
		JSONTools::GetAttribute(json, "red_bits", dst.red_bits);
		JSONTools::GetAttribute(json, "green_bits", dst.green_bits);
		JSONTools::GetAttribute(json, "blue_bits", dst.blue_bits);
		JSONTools::GetAttribute(json, "alpha_bits", dst.alpha_bits);
		return true;
	}

}; // namespace chaos
