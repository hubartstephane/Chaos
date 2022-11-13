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
			client->client_server = nullptr;
			if (client != nullptr)
				client->OnClientDetached(this);
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

}; // namespace chaos
