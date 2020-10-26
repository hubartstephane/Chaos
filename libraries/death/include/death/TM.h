#pragma once

#include <chaos/Chaos.h>

#include <death/Level.h>
#include <death/LevelInstance.h>
#include <death/GameCheckpoint.h>
#include <death/GameFramework.h>

	class Player; // for compilation, nested namespace cause issues

		// ==============================================================
		// FORWARD DECLARATION / FRIENDSHIP MACROS
		// ==============================================================

	namespace death
	{
		// all classes in this file
#define DEATH_TILEDLEVEL_CLASSES \
(TMLevel) \
(TMLevelInstance) \
(TMLayerInstance) \
(TMObject) \
(TMPath) \
(TMCameraTemplate) \
(TMPlayerStart) \
(TMTrigger) \
(TMCheckpointTrigger) \
(TMParticlePopulator) \
(TMTileCollisionIterator)\
(TMSoundTrigger)

		// forward declaration
#define DEATH_TILEDLEVEL_FORWARD_DECL(r, data, elem) class elem;
BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FORWARD_DECL, _, DEATH_TILEDLEVEL_CLASSES)

		// declare templates/using
template<typename T> 
class TMObjectCollisionIteratorBase;

using TMObjectCollisionIterator = TMObjectCollisionIteratorBase<TMObject>;
using TMTriggerCollisionIterator = TMObjectCollisionIteratorBase<TMTrigger>;

		// friendship macro
#define DEATH_TILEDLEVEL_FRIEND_DECL(r, data, elem) friend class elem;
#define DEATH_TILEDLEVEL_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FRIEND_DECL, _, DEATH_TILEDLEVEL_CLASSES)\
	template<typename T> friend class TMObjectCollisionIteratorBase;

	}; // namespace death

		// ==============================================================
		// Internal files
		// ==============================================================

			// XXX : order is important

#include <death/TM/TMParticle.h>
#include <death/TM/TMObject.h>
#include <death/TM/TMLevel.h>
#include <death/TM/TMCollision.h>
#include <death/TM/TMParticlePopulator.h>
#include <death/TM/TMTools.h>

		// ==============================================================
		// Undefs
		// ==============================================================

		 // undefine macros
#undef DEATH_TILEDMAP_CLASSES
#undef DEATH_TILEDMAP_FORWARD_DECL
#undef DEATH_TILEDMAP_FRIEND_DECL
#undef DEATH_TILEDMAP_ALL_FRIENDS


