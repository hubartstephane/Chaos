#pragma once

#include <chaos/StandardHeaders.h>
#include <death/CameraComponent.h>
#include <chaos/Axis.h>

namespace death
{

	// =============================================
	// ScrollCameraComponent
	// =============================================

	class ScrollCameraComponent : public CameraComponent
	{
	public:

		/** constructor */
		ScrollCameraComponent(float in_scroll_speed, chaos::Axis in_axis):
			scroll_speed(in_scroll_speed),
			axis(in_axis){}

		/** get the scroll speed */
		float GetScrollSpeed() const { return scroll_speed;	}
		/** set the scroll speed */
		void SetScrollSpeed(float in_scroll_speed) { scroll_speed = in_scroll_speed; }

		/** get the axis */
		chaos::Axis GetAxis() const { return axis; }

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;

	protected:

		/** the displacement speed */
		float scroll_speed = 0.0f;
		/** the axis of displacement */
		chaos::Axis axis;
	};

}; // namespace death
