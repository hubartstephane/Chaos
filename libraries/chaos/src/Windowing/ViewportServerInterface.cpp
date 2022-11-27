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

}; // namespace chaos
