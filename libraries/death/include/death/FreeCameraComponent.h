#pragma once

#include <Chaos/Chaos.h>
#include <death/CameraComponent.h>

namespace death
{

	// =============================================
	// FreeCameraComponent
	// =============================================

	class FreeCameraComponent : public CameraComponent
	{
	public:

		/** constructor */
		FreeCameraComponent(size_t in_player_index = 0) :
			player_index(in_player_index) {}

		/** returns the zoom value */
		float GetZoomValue() const { return zoom_value; }
		/** set the zoom value */
		void SetZoomValue(float in_zoom_value){ zoom_value = in_zoom_value; }

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual chaos::box2 ApplyModifier(chaos::box2 const & src) const override;

	protected:

		/** the player index to follow */
		size_t player_index = 0;
		/** the value of zoom to apply */
		float zoom_value = 1.0f;
	};

}; // namespace death
