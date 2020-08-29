#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/Object.h>
#include <chaos/TiledMap.h>
#include <chaos/GPURenderable.h>
#include <chaos/Tickable.h>
#include <chaos/ParticleManager.h>
#include <chaos/ParticleDefault.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/CollisionFramework.h>
#include <chaos/SoundManager.h>
#include <chaos/ParticleSpawner.h>
#include <chaos/NamedObject.h>
#include <chaos/JSONSerializable.h>
#include <chaos/Hotpoint.h>

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
(TMCameraTemplate) \
(TMPlayerStart) \
(TMTrigger) \
(TMCheckpointTrigger) \
(TMParticlePopulator) \
(TMTriggerCollisionIterator)\
(TMObjectCollisionIterator)\
(TMTileCollisionIterator)\
(TMSoundTrigger)

		// forward declaration
#define DEATH_TILEDLEVEL_FORWARD_DECL(r, data, elem) class elem;
		BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FORWARD_DECL, _, DEATH_TILEDLEVEL_CLASSES)

		// friendship macro
#define DEATH_TILEDLEVEL_FRIEND_DECL(r, data, elem) friend class elem;
#define DEATH_TILEDLEVEL_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(DEATH_TILEDLEVEL_FRIEND_DECL, _, DEATH_TILEDLEVEL_CLASSES)

	}; // namespace death

		// ==============================================================
		// Internal files
		// ==============================================================

#include <death/TM/TMParticle.h>
#include <death/TM/TMLevel.h>
#include <death/TM/TMObject.h>
#include <death/TM/TMCollision.h>
#include <death/TM/TMParticlePopulator.h>

		// ==============================================================
		// Undefs
		// ==============================================================

		 // undefine macros
#undef DEATH_TILEDMAP_CLASSES
#undef DEATH_TILEDMAP_FORWARD_DECL
#undef DEATH_TILEDMAP_FRIEND_DECL
#undef DEATH_TILEDMAP_ALL_FRIENDS


