#pragma once

#include <chaos/StandardHeaders.h>

#include <chaos/GeometryFramework.h>
#include <chaos/Tickable.h>
#include <death/GameFramework.h>

#include <death/GameGettersDecl.h>

namespace death
{
	// =============================================
	// CameraComponent
	// =============================================

	class CameraComponent : public chaos::Tickable
	{
		DEATH_GAMEFRAMEWORK_ALLFRIENDS;

		CHAOS_OBJECT_DECLARE_CLASS2(CameraComponent, chaos::Tickable);

	public:

		DEATH_GAMEGETTERS_DECLARE();

		/** gets the camera the component belongs to */
		chaos::AutoCastable<Camera> GetCamera() { return camera; }
		/** gets the camera the component belongs to */
		chaos::AutoConstCastable<Camera> GetCamera() const { return camera; }

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
