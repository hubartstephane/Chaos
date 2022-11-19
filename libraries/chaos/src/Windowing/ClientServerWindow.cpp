#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	/**
	* ClientServerWindowInterface implementation
	*/

	AutoCastable<WindowClient> ClientServerWindowInterface::GetClient()
	{
		return client.get();
	}
	AutoConstCastable<WindowClient> ClientServerWindowInterface::GetClient() const
	{
		return client.get();
	}

	void ClientServerWindowInterface::SetClient(WindowClient* in_client)
	{
		if (in_client != client.get())
		{
			// detach previous client
			if (client != nullptr)
			{
				client->client_server = nullptr;
				client->OnClientDetached(this);
			}
			// attach new client
			client = in_client;
			if (client != nullptr)
			{
				client->client_server = this;
				client->OnClientAttached(this);
			}
		}
	}

	/**
	* ClientServerWindow implementation
	*/

	bool ClientServerWindow::OnWindowClosed()
	{
		bool result = true;
		if (client != nullptr)
			result &= client->OnWindowClosed();
		result &= Window::OnWindowClosed();
		return result;
	}

	void ClientServerWindow::OnWindowResize(glm::ivec2 size)
	{
		if (client != nullptr)
			client->OnWindowResize(size);
		Window::OnWindowResize(size);
	}

	void ClientServerWindow::OnDropFile(int count, char const** paths)
	{
		if (client != nullptr)
			client->OnDropFile(count, paths);
		Window::OnDropFile(count, paths);
	}

	void ClientServerWindow::OnIconifiedStateChange(bool iconified)
	{
		if (client != nullptr)
			client->OnIconifiedStateChange(iconified);
		Window::OnIconifiedStateChange(iconified);
	}

	void ClientServerWindow::OnFocusStateChange(bool gain_focus)
	{
		if (client != nullptr)
			client->OnFocusStateChange(gain_focus);
		Window::OnFocusStateChange(gain_focus);
	}

	bool ClientServerWindow::OnDraw(GPURenderer* renderer, WindowDrawParams const& DrawParams, GPUProgramProviderInterface const* uniform_provider)
	{
		bool result = false;
		if (client != nullptr)
			result |= client->OnDraw(renderer, DrawParams, uniform_provider);
		result |= Window::OnDraw(renderer, DrawParams, uniform_provider);
		return result;
	}

	bool ClientServerWindow::OnMouseMoveImpl(double x, double y)
	{
		if (client != nullptr)
			if (client->OnMouseMove(x, y))
				return true;
		return Window::OnMouseMoveImpl(x, y);
	}

	bool ClientServerWindow::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (client != nullptr)
			if (client->OnMouseButton(button, action, modifier))
				return true;
		return Window::OnMouseButtonImpl(button, action, modifier);
	}

	bool ClientServerWindow::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		if (client != nullptr)
			if (client->OnMouseWheel(scroll_x, scroll_y))
				return true;
		return Window::OnMouseWheelImpl(scroll_x, scroll_y);
	}
	
	bool ClientServerWindow::OnKeyEventImpl(KeyEvent const& event)
	{
		if (client != nullptr)
			if (client->OnKeyEvent(event))
				return true;
		return Window::OnKeyEventImpl(event);
	}

	bool ClientServerWindow::OnCharEventImpl(unsigned int c)
	{
		if (client != nullptr)
			if (client->OnCharEvent(c))
				return true;
		return Window::OnCharEventImpl(c);
	}

	bool ClientServerWindow::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const
	{
		if (client != nullptr)
			if (client->DoProcessAction(execution_data))
				return true;
		return Window::DoProcessAction(execution_data);
	}

	bool ClientServerWindow::DoTick(float delta_time)
	{
		bool result = false;
		if (client != nullptr)
			result |= client->Tick(delta_time);
		result |= TickableInterface::DoTick(delta_time);
		return result;
	}

}; // namespace chaos
