#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	/**
	* ViewportServerInterface implementation
	*/

	size_t ViewportServerInterface::GetViewportCount() const
	{
		return viewports.size();
	}

	AutoCastable<Viewport> ViewportServerInterface::GetViewport(size_t index)
	{
		return viewports[index].get();
	}

	AutoConstCastable<Viewport> ViewportServerInterface::GetViewport(size_t index) const
	{
		return viewports[index].get();
	}

	AutoCastable<Viewport> ViewportServerInterface::FindViewport(ObjectRequest request)
	{
		return request.FindObject(viewports);
	}

	AutoConstCastable<Viewport> ViewportServerInterface::FindViewport(ObjectRequest request) const
	{
		return request.FindObject(viewports);
	}

	void ViewportServerInterface::AttachViewport(Viewport * viewport)
	{
		viewport->viewport_server = this;
		viewport->OnAttachedToServer(this);
	}

	void ViewportServerInterface::DetachViewport(Viewport* viewport)
	{
		viewport->viewport_server = nullptr;
		viewport->OnDetachedFromServer(this);
	}

}; // namespace chaos
