#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	AutoCastable<ViewportServerWindowInterface> Viewport::GetViewportServer()
	{
		return viewport_server;
	}

	AutoConstCastable<ViewportServerWindowInterface> Viewport::GetViewportServer() const
	{
		return viewport_server;
	}

	void Viewport::OnClientDetached(ViewportServerWindowInterface * in_viewport_server)
	{

	}

	void Viewport::OnClientAttached(ViewportServerWindowInterface* in_viewport_server)
	{

	}

}; // namespace chaos
