#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ImGuiLogObject::ImGuiLogObject()
	{
		SetLogger(Logger::GetInstance());
	}

	void ImGuiLogObject::SetLogger(Logger * in_logger)
	{
		logger = in_logger;
	}

	void ImGuiLogObject::OnDrawImGuiContent(Window* window)
	{
		assert(logger != nullptr);

		// the lines
		int table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Reorderable;

		ImGuiTools::DrawImGuiTable("##lines", table_flags, "Date", "Severity", "Domain", "Count", "Message", "Action")([&]()
		{
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
				if (line.severity == LogSeverity::Message)
				{
					if (!show_messages)
						continue;
					color = { 1.0f, 1.0f, 1.0f, 1.0f };
				}
				else if (line.severity == LogSeverity::Warning)
				{
					if (!show_warnings)
						continue;
					color = { 1.0f, 0.64f, 0.0f, 1.0f };
				}
				else if (line.severity == LogSeverity::Error)
				{
					if (!show_errors)
						continue;
					color = { 1.0f, 0.0f, 0.0f, 1.0f };
				}

				// filter by content
				if (!filter.PassFilter(line.content.c_str()))
					continue;

				size_t constexpr COLUMN_COUNT = 6;

				// time
				ImGui::PushID(int(i * COLUMN_COUNT + 0));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, "%s", StringTools::TimeToString(line.time, TimeToStringFormatType::FULL).c_str());
				ImGui::PopID();

				// type
				ImGui::PushID(int(i * COLUMN_COUNT + 1));
				ImGui::TableNextColumn();
				ImGui::TextColored(color, EnumToString(line.severity));
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
					ImGui::SetClipboardText(line.ToString().c_str());

				ImGui::PopID();
			}
		});
	}

	void ImGuiLogObject::OnDrawImGuiMenu(Window* window, BeginImGuiMenuFunc begin_menu_func)
	{
		begin_menu_func([this]()
		{
			// the types
			if (ImGui::BeginMenu("Severity"))
			{
				ImGui::Checkbox("messages", &show_messages);
				ImGui::Checkbox("warnings", &show_warnings);
				ImGui::Checkbox("errors", &show_errors);
				ImGui::EndMenu();
			}
			// the domains
			if (ImGui::BeginMenu("Domains"))
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
					ImGui::Checkbox(domain, domain_flag);
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
					if (LoggerListener* logger_listener = logger->GetListener(i))
						logger_listener->DrawImGuiMenu();
				ImGui::EndMenu();
			}
			// filter
			if (ImGui::BeginMenu("Filter"))
			{
				filter.Draw();
				ImGui::EndMenu();
			}
		});
	}

}; // namespace chaos
