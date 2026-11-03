#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	DrawImGuiTable::DrawImGuiTable(char const* title, std::optional<int> table_flags, std::span<std::string_view> column_names)
	{
		table_opened = ImGuiTools::OpenTable(title, table_flags, column_names);
	}

	DrawImGuiTable::~DrawImGuiTable()
	{
		if (table_opened)
		{
			ImGui::EndTable();
			table_opened = false;
		}
	}

	DrawImGuiTable const& DrawImGuiTable::operator ()(LightweightFunction<void()> func) const
	{
		func();
		return *this;
	}

}; // namespace chaos