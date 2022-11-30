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

	size_t ViewportServerInterface::FindViewportIndex(Viewport const * viewport) const
	{
		assert(viewport->GetViewportServer() == this); // used by us

		auto it = std::find(viewports.begin(), viewports.end(), viewport);
		assert(it != viewports.end());
		return it - viewports.begin();
	}

	void ViewportServerInterface::AttachViewport(Viewport * viewport)
	{
		assert(viewport->GetViewportServer() == nullptr); // not used anywhere

		viewport->viewport_server = this;
		viewport->OnAttachedToServer(this);
	}

	void ViewportServerInterface::DetachViewport(Viewport* viewport)
	{
		assert(viewport->GetViewportServer() == this); // used by us

		viewport->viewport_server = nullptr;
		viewport->OnDetachedFromServer(this);
	}

}; // namespace chaos
