#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	// all classes in this file
#define CHAOS_GAMEPLAY_TM_CLASSES \
(TMLevel) \
(TMLevelInstance) \
(TMLayerInstance) \
(TMObject) \
(TMPath) \
(TMCameraTemplate) \
(TMPlayerStart) \
(TMTrigger) \
(TMObjectReferenceSolver)\
(TMNotificationTrigger)\
(TMChangeLevelTrigger)\
(TMCheckpointTrigger) \
(TMParticlePopulator) \
(TMTileCollisionIterator)\
(TMTriggerCollisionInfo)\
(TMSoundTrigger)\
(TMParticle)\
(TMParticleLayerTrait)\
(TMParticlePopulator)\
(TileCollisionInfo)\
(TileCollisionComputer)\
(TMCollisionIteratorBase)\
(TMLayerInstanceIterator)\
(TMTileCollisionIterator)

		// forward declaration
#define CHAOS_GAMEPLAY_TM_FORWARD_DECL(r, data, elem) class elem;
BOOST_PP_SEQ_FOR_EACH(CHAOS_GAMEPLAY_TM_FORWARD_DECL, _, CHAOS_GAMEPLAY_TM_CLASSES)

		// declare templates/using
	template<typename T>
	class TMObjectCollisionIteratorBase;

	using TMObjectCollisionIterator = TMObjectCollisionIteratorBase<TMObject>;
	using TMTriggerCollisionIterator = TMObjectCollisionIteratorBase<TMTrigger>;

	// friendship macro
#define CHAOS_GAPMEPLAY_TM_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_GAMEPLAY_TM_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_GAPMEPLAY_TM_FRIEND_DECL, _, CHAOS_GAMEPLAY_TM_CLASSES)\
	template<typename T> friend class TMObjectCollisionIteratorBase;


}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

#include <chaos/Gameplay/TM/TMParticle.h>
#include <chaos/Gameplay/TM/TMObject.h>
#include <chaos/Gameplay/TM/TMLevel.h>
#include <chaos/Gameplay/TM/TMLayerInstanceIterator.h>
#include <chaos/Gameplay/TM/TMCollision.h>
#include <chaos/Gameplay/TM/TMParticlePopulator.h>
#include <chaos/Gameplay/TM/TMTools.h>
