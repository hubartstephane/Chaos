#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiChoiceObject::ImGuiChoiceObject(char const** in_choices, std::function<void(size_t)> in_choice_function) :
		choice_function(std::move(in_choice_function))
	{
		size_t i = 0;
		while (in_choices[i] != nullptr)
		{
			choices.emplace_back(in_choices[i]);
			++i;
		}
	}

	void ImGuiChoiceObject::OnDrawImGuiContent(Window* window)
	{
		for (size_t i = 0; i < choices.size(); ++i)
			if (ImGui::Selectable(choices[i].c_str()))
				if (choice_function)
					choice_function(i);
	}

}; // namespace chaos