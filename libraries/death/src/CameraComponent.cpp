#include <death/CameraComponent.h>

namespace death
{

	// =================================================
	// CameraComponent
	// =================================================

	CameraComponent::CameraComponent()
	{	
	}

	void CameraComponent::OnInsertedInto(Camera * in_camera)
	{
		assert(in_camera != nullptr);
	}

	void CameraComponent::OnRemovedFrom(Camera * in_camera)
	{
		assert(in_camera != nullptr);		
	}

	// =============================================
	// FollowPlayerCameraComponent
	// =============================================

	bool FollowPlayerCameraComponent::DoTick(double delta_time)
	{

		return true;
	}

}; // namespace death
