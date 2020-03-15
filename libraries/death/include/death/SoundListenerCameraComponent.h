#pragma once

#include <chaos/StandardHeaders.h>
#include <death/CameraComponent.h>

namespace death
{
	// =============================================
	// SoundListenerCameraComponent
	// =============================================

	class SoundListenerCameraComponent : public CameraComponent
	{
	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
	};

}; // namespace death
