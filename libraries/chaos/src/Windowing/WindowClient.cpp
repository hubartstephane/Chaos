#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	AutoCastable<ClientServerWindowInterface> WindowClient::GetClientServer()
	{
		return client_server;
	}

	AutoConstCastable<ClientServerWindowInterface> WindowClient::GetClientServer() const
	{
		return client_server;
	}

	void WindowClient::OnClientDetached(ClientServerWindowInterface * in_client_server)
	{

	}

	void WindowClient::OnClientAttached(ClientServerWindowInterface* in_client_server)
	{

	}

}; // namespace chaos
