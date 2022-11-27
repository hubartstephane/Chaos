#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	/**
	* ViewportServerInterface implementation
	*/

	AutoCastable<Viewport> ViewportServerInterface::GetViewport()
	{
		return viewport.get();
	}
	AutoConstCastable<Viewport> ViewportServerInterface::GetViewport() const
	{
		return viewport.get();
	}

	void ViewportServerInterface::SetViewport(Viewport* in_viewport)
	{
		if (in_viewport != viewport.get())
		{
			// detach previous client
			if (viewport != nullptr)
			{
				viewport->viewport_server = nullptr;
				viewport->OnDetachedFromServer(this);
			}
			// attach new client
			viewport = in_viewport;
			if (viewport != nullptr)
			{
				viewport->viewport_server = this;
				viewport->OnAttachedToServer(this);
			}
		}
	}

	/**
	* ViewportServerWindow implementation
	*/

	bool ViewportServerWindow::OnWindowClosed()
	{
		bool result = true;
		if (viewport != nullptr)
			result &= viewport->OnWindowClosed();
		result &= Window::OnWindowClosed();
		return result;
	}

	void ViewportServerWindow::OnWindowResize(glm::ivec2 size)
	{
		if (viewport != nullptr)
			viewport->OnWindowResize(size);
		Window::OnWindowResize(size);
	}

	void ViewportServerWindow::OnDropFile(int count, char const** paths)
	{
		if (viewport != nullptr)
			viewport->OnDropFile(count, paths);
		Window::OnDropFile(count, paths);
	}

	void ViewportServerWindow::OnIconifiedStateChange(bool iconified)
	{
		if (viewport != nullptr)
			viewport->OnIconifiedStateChange(iconified);
		Window::OnIconifiedStateChange(iconified);
	}

	void ViewportServerWindow::OnFocusStateChange(bool gain_focus)
	{
		if (viewport != nullptr)
			viewport->OnFocusStateChange(gain_focus);
		Window::OnFocusStateChange(gain_focus);
	}

	bool ViewportServerWindow::OnDraw(GPURenderer* renderer, WindowDrawParams const& DrawParams, GPUProgramProviderInterface const* uniform_provider)
	{
		bool result = false;
		if (viewport != nullptr)
			result |= viewport->OnDraw(renderer, DrawParams, uniform_provider);
		result |= Window::OnDraw(renderer, DrawParams, uniform_provider);
		return result;
	}

	bool ViewportServerWindow::OnMouseMoveImpl(double x, double y)
	{
		if (viewport != nullptr)
			if (viewport->OnMouseMove(x, y))
				return true;
		return Window::OnMouseMoveImpl(x, y);
	}

	bool ViewportServerWindow::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (viewport != nullptr)
			if (viewport->OnMouseButton(button, action, modifier))
				return true;
		return Window::OnMouseButtonImpl(button, action, modifier);
	}

	bool ViewportServerWindow::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		if (viewport != nullptr)
			if (viewport->OnMouseWheel(scroll_x, scroll_y))
				return true;
		return Window::OnMouseWheelImpl(scroll_x, scroll_y);
	}
	
	bool ViewportServerWindow::OnKeyEventImpl(KeyEvent const& event)
	{
		if (viewport != nullptr)
			if (viewport->OnKeyEvent(event))
				return true;
		return Window::OnKeyEventImpl(event);
	}

	bool ViewportServerWindow::OnCharEventImpl(unsigned int c)
	{
		if (viewport != nullptr)
			if (viewport->OnCharEvent(c))
				return true;
		return Window::OnCharEventImpl(c);
	}

	bool ViewportServerWindow::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const
	{
		if (viewport != nullptr)
			if (viewport->DoProcessAction(execution_data))
				return true;
		return Window::DoProcessAction(execution_data);
	}

	bool ViewportServerWindow::DoTick(float delta_time)
	{
		bool result = false;
		if (viewport != nullptr)
			result |= viewport->Tick(delta_time);
		result |= TickableInterface::DoTick(delta_time);
		return result;
	}

}; // namespace chaos
