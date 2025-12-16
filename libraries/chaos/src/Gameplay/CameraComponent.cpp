#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	// =================================================
	// CameraComponent
	// =================================================

	CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(CameraComponent);

	box2 CameraComponent::ApplyModifier(box2 const & src) const
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

	bool CameraComponent::TraverseInputReceiver(InputReceiverTraverser& in_traverser, InputDeviceInterface const* in_input_device)
	{
		return InputReceiverInterface::TraverseInputReceiver(in_traverser, in_input_device);
	}

	bool CameraComponent::EnumerateInputActions(InputActionEnumerator& in_action_enumerator, EnumerateInputActionContext in_context)
	{
		return InputReceiverInterface::EnumerateInputActions(in_action_enumerator, in_context);
	}

}; // namespace chaos
