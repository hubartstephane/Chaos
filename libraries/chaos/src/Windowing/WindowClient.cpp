#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void WindowClient::OnPluggedIntoWindow(Window* in_window)
	{
	}

	void WindowClient::OnUnpluggedFromWindow(Window* in_window)
	{
	}

#if _WIN32

	void WindowClient::OnInputLanguageChanged()
	{
	}

#endif // #if _WIN32

	void WindowClient::OnIconifiedStateChange(bool iconified)
	{
	}

	void WindowClient::OnFocusStateChange(bool gain_focus)
	{
	}

	void WindowClient::OnMonitorEvent(GLFWmonitor* monitor, int monitor_state)
	{
	}

	void WindowClient::OnDropFile(int count, char const** paths)
	{
	}

	bool WindowClient::OnDraw(GPURenderContext* render_context, GPUProgramProviderInterface const* uniform_provider, WindowDrawParams const& draw_params)
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);
		return true;
	}

	void WindowClient::OnWindowResize(glm::ivec2 size)
	{
	}

	bool WindowClient::OnWindowClosed()
	{
		return true;
	}

}; // namespace chaos
