#include <death/CameraComponent.h>
#include <death/Camera.h>
#include <death/Player.h>

namespace death
{

	// =================================================
	// CameraComponent
	// =================================================

	CameraComponent::CameraComponent()
	{	
	}

	chaos::box2 CameraComponent::ApplyModifier(chaos::box2 const & src) const
	{
		return src;
	}

	void CameraComponent::OnInsertedInto(Camera * in_camera)
	{
		assert(in_camera != nullptr);
	}

	void CameraComponent::OnRemovedFrom(Camera * in_camera)
	{
		assert(in_camera != nullptr);		
	}

}; // namespace death
