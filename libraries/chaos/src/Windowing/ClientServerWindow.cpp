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
		if (client != nullptr)
			return client->OnWindowClosed();
		return false;
	}

	void ClientServerWindow::OnWindowResize(glm::ivec2 size)
	{
		if (client != nullptr)
			client->OnWindowResize(size);
	}

	void ClientServerWindow::OnDropFile(int count, char const** paths)
	{
		if (client != nullptr)
			client->OnDropFile(count, paths);
	}

	void ClientServerWindow::OnIconifiedStateChange(bool iconified)
	{
		if (client != nullptr)
			client->OnIconifiedStateChange(iconified);
	}

	void ClientServerWindow::OnFocusStateChange(bool gain_focus)
	{
		if (client != nullptr)
			client->OnFocusStateChange(gain_focus);
	}

	bool ClientServerWindow::OnDraw(GPURenderer* renderer, WindowDrawParams const& DrawParams, GPUProgramProviderInterface const* uniform_provider)
	{
		if (client != nullptr)
			return client->OnDraw(renderer, DrawParams, uniform_provider);
		return false;
	}

	bool ClientServerWindow::OnMouseMoveImpl(double x, double y)
	{
		if (client != nullptr)
			return client->OnMouseMove(x, y);
		return false;
	}

	bool ClientServerWindow::OnMouseButtonImpl(int button, int action, int modifier)
	{
		if (client != nullptr)
			return client->OnMouseButton(button, action, modifier);
		return false;
	}

	bool ClientServerWindow::OnMouseWheelImpl(double scroll_x, double scroll_y)
	{
		if (client != nullptr)
			return client->OnMouseWheel(scroll_x, scroll_y);
		return false;
	}
	
	bool ClientServerWindow::OnKeyEventImpl(KeyEvent const& event)
	{
		if (client != nullptr)
			return client->OnKeyEvent(event);
		return false;
	}

	bool ClientServerWindow::OnCharEventImpl(unsigned int c)
	{
		if (client != nullptr)
			return client->OnCharEvent(c);
		return false;
	}

	bool ClientServerWindow::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const
	{
		if (client != nullptr)
			return client->DoProcessAction(execution_data);
		return false;
	}

	bool ClientServerWindow::DoTick(float delta_time)
	{
		if (client != nullptr)
			return client->Tick(delta_time);
		return TickableInterface::DoTick(delta_time);
	}

}; // namespace chaos
