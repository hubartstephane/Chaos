#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiCustomObject::ImGuiCustomObject(std::function<void()> in_draw_function) :
		draw_function(std::move(in_draw_function))
	{
	}

	ImGuiCustomObject::ImGuiCustomObject(std::function<void(Window*)> in_draw_function_with_window) :
		draw_function_with_window(std::move(in_draw_function_with_window))
	{
	}

	void ImGuiCustomObject::OnDrawImGuiContent(Window* window)
	{
		if (draw_function)
			draw_function();
		else if (draw_function_with_window)
			draw_function_with_window(window);
	}

}; // namespace chaos