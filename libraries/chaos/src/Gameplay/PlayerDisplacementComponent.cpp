#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_GAMEPLAY_GETTERS(PlayerDisplacementComponent);

	// =============================================
	// PlayerDisplacementComponent
	// =============================================

	bool PlayerDisplacementComponent::Initialize(Player* in_player)
	{
		// ensure valid arguments and not already initialized
		assert(in_player != nullptr);
		assert(player == nullptr);
		player = in_player;
		return true;
	}

	bool PlayerDisplacementComponent::TraverseInputReceiver(InputReceiverTraverser& in_traverser, InputDeviceInterface const* in_input_device)
	{
		return InputReceiverInterface::TraverseInputReceiver(in_traverser, in_input_device);
	}

	bool PlayerDisplacementComponent::EnumerateInputActions(InputActionEnumerator& in_action_enumerator, EnumerateInputActionContext in_context)
	{
		return InputReceiverInterface::EnumerateInputActions(in_action_enumerator, in_context);
	}

}; // namespace chaos