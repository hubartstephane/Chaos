#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void LoggerImGuiContent::SetLogger(Logger * in_logger)
	{
		logger = in_logger;
	}

	void LoggerImGuiContent::OnDrawImGuiMenu()
	{
		// the types
		if (ImGui::BeginMenu("Type"))
		{
			ImGui::Checkbox("messages", &show_messages); ImGui::SameLine();
			ImGui::Checkbox("warnings", &show_warnings); ImGui::SameLine();
			ImGui::Checkbox("errors", &show_errors);
			ImGui::EndMenu();
		}
		// the domains
		if (ImGui::BeginMenu("Domains"))
		{
			if (ImGui::BeginTable("split", 3))
			{
				for (size_t i = 0; i < logger->GetDomainCount(); ++i)
				{
					char const* domain = logger->GetDomain(i);
					bool* domain_flag = nullptr;

					auto it = domain_visibilities.find(domain);
					if (it != domain_visibilities.end())
					{
						domain_flag = &it->second;
					}
					else
					{
						domain_visibilities[domain] = true;
						domain_flag = &domain_visibilities.find(domain)->second;
					}
					ImGui::TableNextColumn(); ImGui::Checkbox(domain, domain_flag);
				}
				ImGui::EndTable();
			}
			ImGui::EndMenu();
		}
		// Miscellaneaous
		if (ImGui::BeginMenu("Miscellaneous"))
		{
			ImGui::Checkbox("group identical lines", &group_identical_lines);
			ImGui::EndMenu();
		}
		// some listeners' actions
		if (ImGui::BeginMenu("Actions"))
		{
			for (size_t i = 0; i < logger->GetListenerCount(); ++i)
				if (ImGuiDrawableInterface* imgui_drawable = auto_cast(logger->GetListener(i)))
					imgui_drawable->OnDrawImGuiMenu();
			ImGui::EndMenu();
		}
		// filter
		if (ImGui::BeginMenu("Filter"))
		{
			filter.Draw();
			ImGui::EndMenu();
		}
	}

	void LoggerImGuiContent::OnDrawImGuiContent()
	{
		assert(logger != nullptr);

		// the lines
		size_t constexpr COLUMN_COUNT = 6;

		if (ImGui::BeginTable("##lines", COLUMN_COUNT, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Reorderable))
		{
			ImGui::TableSetupColumn("Date", 0);
			ImGui::TableSetupColumn("Type", 0);
			ImGui::TableSetupColumn("Domain", 0);
			ImGui::TableSetupColumn("Count", 0);
			ImGui::TableSetupColumn("Message", 0);
			ImGui::TableSetupColumn("Action", 0);
			ImGui::TableHeadersRow();

			std::vector<LogLine> const& lines = logger->GetLines();
			for (size_t i = 0; i < lines.size(); ++i)
			{
				LogLine const& line = lines[i];

				// group messages
				size_t group_count = 1;
				if (group_identical_lines)
				{
					while (i + 1 < lines.size() && line.IsComparable(lines[i + 1]))
					{
						++i;
						++group_count;
					}
				}

				// filter out by domain
				auto it = domain_visibilities.find(line.domain);
				if (it != domain_visibilities.end())
					if (!it->second)
						continue;

				// search color and filter out by type
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

				// filter by content
				if (!filter.PassFilter(line.content.c_str()))
					continue;

				// time
				ImGui::PushID(int(i * COLUMN_COUNT + 0));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, "%s", StringTools::TimeToString(line.time, TimeToStringFormatType::FULL).c_str());
				ImGui::PopID();

				// type
				ImGui::PushID(int(i * COLUMN_COUNT + 1));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, EnumToString(line.type));
				ImGui::PopID();

				// domain
				ImGui::PushID(int(i * COLUMN_COUNT + 2));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, "%s", line.domain);
				ImGui::PopID();

				// group count
				ImGui::PushID(int(i * COLUMN_COUNT + 3));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, "%d", group_count);
				ImGui::PopID();

				// message
				ImGui::PushID(int(i * COLUMN_COUNT + 4));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, "%s", line.content.c_str());
				ImGui::PopID();

				// actions
				ImGui::PushID(int(i * COLUMN_COUNT + 5));
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
