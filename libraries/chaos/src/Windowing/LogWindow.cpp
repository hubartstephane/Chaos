#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	LogWindow::LogWindow()
	{
		if (ImGuiLogObject* new_content = new ImGuiLogObject)
		{
			content = new_content;
			new_content->SetLogger(Logger::GetInstance());
		}
	}

}; // namespace chaos
