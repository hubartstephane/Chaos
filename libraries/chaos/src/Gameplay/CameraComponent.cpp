#include <chaos/Chaos.h>

namespace chaos
{

	// =================================================
	// CameraComponent
	// =================================================

	CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(CameraComponent);

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

}; // namespace chaos
