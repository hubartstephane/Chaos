#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	AutoCastable<ViewportServerInterface> Viewport::GetViewportServer()
	{
		return viewport_server;
	}

	AutoConstCastable<ViewportServerInterface> Viewport::GetViewportServer() const
	{
		return viewport_server;
	}

	void Viewport::OnDetachedFromServer(ViewportServerInterface * in_viewport_server)
	{

	}

	void Viewport::OnAttachedToServer(ViewportServerInterface* in_viewport_server)
	{

	}

	AutoCastable<ViewportServerWindow> Viewport::GetServerWindow()
	{
		ViewportServerInterface* parent = viewport_server;
		while (parent != nullptr)
		{
			if (ViewportServerWindow* window = auto_cast(parent))
				return window;
			if (Viewport* viewport = auto_cast(parent))
				parent = viewport->viewport_server;
			else
				return nullptr;
		}
		return nullptr;
	}

	AutoConstCastable<ViewportServerWindow> Viewport::GetServerWindow() const
	{
		ViewportServerInterface const* parent = viewport_server;
		while (parent != nullptr)
		{
			if (ViewportServerWindow const* window = auto_cast(parent))
				return window;
			if (Viewport const* viewport = auto_cast(parent))
				parent = viewport->viewport_server;
			else
				return nullptr;
		}
		return nullptr;
	}

}; // namespace chaos
