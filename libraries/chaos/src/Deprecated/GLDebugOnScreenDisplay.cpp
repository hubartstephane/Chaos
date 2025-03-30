#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void GLDebugOnScreenDisplay::OnDrawImGuiContent(Window* window)
	{
		ImGuiIO& io = ImGui::GetIO();
		float delta_time = io.DeltaTime;

		// destroy outdated lines
		auto it = std::remove_if(lines.begin(), lines.end(), [delta_time](ImGuiUserMessageLine & line)
		{
			// infinite lifetime ?
			if (line.remaining_lifetime <= 0)
				return false;
			// decrement lifetime
			line.remaining_lifetime -= delta_time;
			// check whether end is reached
			return (line.remaining_lifetime <= 0); 
		});

		lines.erase(it, lines.end());

		// display lines
		for (ImGuiUserMessageLine const& line : lines)
		{
			ImGui::TextColored(
				{line.color.r, line.color.g, line.color.b, line.color.a},
				"%s", line.content.c_str()
			);
		}
	}

	void GLDebugOnScreenDisplay::AddLine(std::string content, float lifetime , glm::vec4 const& color)
	{
		lines.push_back({std::move(content), lifetime, color});
	}

	void GLDebugOnScreenDisplay::Clear()
	{
		lines.clear();
	}

}; // namespace chaos
