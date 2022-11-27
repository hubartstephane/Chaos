#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	/**
	* ViewportServerWindow implementation
	*/

	AutoCastable<Viewport> ViewportServerWindow::GetViewport()
	{
		if (viewports.size() == 0)
			return nullptr;
		return viewports[0].get();
	}

	AutoConstCastable<Viewport> ViewportServerWindow::GetViewport() const
	{
		if (viewports.size() == 0)
			return nullptr;
		return viewports[0].get();
	}

	void ViewportServerWindow::SetViewport(Viewport* in_viewport)
	{
		Viewport* current_viewport = GetViewport();

		if (in_viewport != current_viewport)
		{
			// detach previous client
			if (current_viewport != nullptr)
			{
				DetachViewport(current_viewport);
;				viewports.pop_back();
			}
			// attach new client
			if (in_viewport != nullptr)
			{
				viewports.push_back(in_viewport);
				AttachViewport(in_viewport);
			}
		}
	}

	bool ViewportServerWindow::OnWindowClosed()
	{
		bool result = true;
		if (Viewport * viewport = GetViewport())
			result &= viewport->OnWindowClosed();
		result &= Window::OnWindowClosed();
		return result;
	}

	void ViewportServerWindow::OnWindowResize(glm::ivec2 size)
	{
		if (Viewport* viewport = GetViewport())
			viewport->OnWindowResize(size);
		Window::OnWindowResize(size);
	}

	void ViewportServerWindow::OnDropFile(int count, char const** paths)
	{
		if (Viewport* viewport = GetViewport())
			viewport->OnDropFile(count, paths);
		Window::OnDropFile(count, paths);
	}

	void ViewportServerWindow::OnIconifiedStateChange(bool iconified)
	{
		if (Viewport* viewport = GetViewport())
			viewport->OnIconifiedStateChange(iconified);
		Window::OnIconifiedStateChange(iconified);
	}

	void ViewportServerWindow::OnFocusStateChange(bool gain_focus)
	{
		if (Viewport* viewport = GetViewport())
			viewport->OnFocusStateChange(gain_focus);
		Window::OnFocusStateChange(gain_focus);
	}

	bool ViewportServerWindow::OnDraw(GPURenderer* renderer, WindowDrawParams const& DrawParams, GPUProgramProviderInterface const* uniform_provider)
	{
		bool result = false;
		if (Viewport* viewport = GetViewport())
			result |= viewport->OnDraw(renderer, DrawParams, uniform_provider);
		result |= Window::OnDraw(renderer, DrawParams, uniform_provider);
		return result;
	}

	bool ViewportServerWindow::OnMouseMoveImpl(double x, double y)
	{
		if (Viewport* viewport = GetViewport())
			if (viewport->OnMouseMove(x, y))
				return true;
		return Window::OnMouseMoveImpl(x, y);
	}

	bool ViewportServerWindow::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (Viewport* viewport = GetViewport())
			if (viewport->OnMouseButton(button, action, modifier))
				return true;
		return Window::OnMouseButtonImpl(button, action, modifier);
	}

	bool ViewportServerWindow::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		if (Viewport* viewport = GetViewport())
			if (viewport->OnMouseWheel(scroll_x, scroll_y))
				return true;
		return Window::OnMouseWheelImpl(scroll_x, scroll_y);
	}
	
	bool ViewportServerWindow::OnKeyEventImpl(KeyEvent const& event)
	{
		if (Viewport* viewport = GetViewport())
			if (viewport->OnKeyEvent(event))
				return true;
		return Window::OnKeyEventImpl(event);
	}

	bool ViewportServerWindow::OnCharEventImpl(unsigned int c)
	{
		if (Viewport* viewport = GetViewport())
			if (viewport->OnCharEvent(c))
				return true;
		return Window::OnCharEventImpl(c);
	}

	bool ViewportServerWindow::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const
	{
		if (Viewport const* viewport = GetViewport())
			if (viewport->DoProcessAction(execution_data))
				return true;
		return Window::DoProcessAction(execution_data);
	}

	bool ViewportServerWindow::DoTick(float delta_time)
	{
		bool result = false;
		if (Viewport* viewport = GetViewport())
			result |= viewport->Tick(delta_time);
		result |= TickableInterface::DoTick(delta_time);
		return result;
	}

}; // namespace chaos
