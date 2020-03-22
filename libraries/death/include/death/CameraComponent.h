#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GeometryFramework.h>
#include <chaos/Tickable.h>
#include <death/GameFramework.h>

namespace death
{
	// =============================================
	// CameraComponent
	// =============================================

	class CameraComponent : public chaos::Tickable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS()

	public:

		/** constructor */
		CameraComponent();

		/** gets the camera the component belongs to */
		Camera* GetCamera() { return camera; }
		/** gets the camera the component belongs to */
		Camera const * GetCamera() const { return camera; }

		/** modifier */
		virtual chaos::box2 ApplyModifier(chaos::box2 const & src) const;

	protected:

		/** called whenever the component is inserted into the camera */
		virtual void OnInsertedInto(Camera * in_camera);
		/** called whenever the component is removed from the camera */
		virtual void OnRemovedFrom(Camera * in_camera);

	protected:

		/** the owning camera */
		Camera * camera = nullptr;
	};


}; // namespace death
