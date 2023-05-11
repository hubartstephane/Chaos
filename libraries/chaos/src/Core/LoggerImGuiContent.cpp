#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void LoggerImGuiContent::SetLogger(Logger * in_logger)
	{
		logger = in_logger;
	}

	void LoggerImGuiContent::DrawImGui()
	{
		assert(logger != nullptr);

		ImGui::Checkbox("messages", &show_messages); ImGui::SameLine();
		ImGui::Checkbox("warnings", &show_warnings); ImGui::SameLine();
		ImGui::Checkbox("errors", &show_errors);
		ImGui::Checkbox("group identical lines", &group_identical_lines);

		for (size_t i = 0; i < logger->GetListenerCount(); ++i)
			if (ImGuiDrawableInterface * imgui_drawable = auto_cast(logger->GetListener(i)))
				imgui_drawable->DrawImGui();

		size_t constexpr COLUMN_COUNT = 5;

		if (ImGui::BeginTable("##lines", COLUMN_COUNT, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableSetupColumn("Date", 0);
			ImGui::TableSetupColumn("Type", 0);
			ImGui::TableSetupColumn("Count", 0);
			ImGui::TableSetupColumn("Message", 0);
			ImGui::TableSetupColumn("Action", 0);
			ImGui::TableHeadersRow();

			std::vector<LogLine> const& lines = logger->GetLines();
			for (size_t i = 0; i < lines.size(); ++i)
			{
				LogLine const& line = lines[i];

				size_t group_count = 1;
				if (group_identical_lines)
				{
					while (i + 1 < lines.size() && line.IsComparable(lines[i + 1]))
					{
						++i;
						++group_count;
					}
				}

				ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
				if (line.type == LogType::Message)
				{
					if (!show_messages)
						continue;
					color = { 1.0f, 1.0f, 1.0f, 1.0f };
				}
				else if (line.type == LogType::Warning)
				{
					if (!show_warnings)
						continue;
					color = { 1.0f, 0.64f, 0.0f, 1.0f };
				}
				else if (line.type == LogType::Error)
				{
					if (!show_errors)
						continue;
					color = { 1.0f, 0.0f, 0.0f, 1.0f };
				}
				char const* message_type = EnumToString(line.type);

				ImGui::PushID(int(i * COLUMN_COUNT + 0));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, "%d", line.time);
				ImGui::PopID();

				ImGui::PushID(int(i * COLUMN_COUNT + 1));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, message_type);
				ImGui::PopID();

				ImGui::PushID(int(i * COLUMN_COUNT + 2));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, "%d", group_count);
				ImGui::PopID();

				ImGui::PushID(int(i * COLUMN_COUNT + 3));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, "%s", line.content.c_str());
				ImGui::PopID();

				ImGui::PushID(int(i * COLUMN_COUNT + 4));
				ImGui::TableNextColumn();
				if (ImGui::Button("Clipboard"))
				{
					ImGui::GetIO().SetClipboardTextFn(nullptr, line.ToString().c_str());
				}
				ImGui::PopID();

			}
			ImGui::EndTable();
		}
	}

}; // namespace chaos
