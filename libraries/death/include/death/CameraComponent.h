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

	protected:

		/** the owning camera */
		Camera * camera = nullptr;

	};

}; // namespace death
