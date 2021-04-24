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
(TMTriggerCollisionInfo)\
(TMSoundTrigger)\
(TMParticle)\
(TMParticleLayerTrait)\
(TMParticlePopulator)\
(TileCollisionComputer)

		// forward declaration
#define CHAOS_GAMEPLAY_TM_FORWARD_DECL(r, data, elem) class elem;
BOOST_PP_SEQ_FOR_EACH(CHAOS_GAMEPLAY_TM_FORWARD_DECL, _, CHAOS_GAMEPLAY_TM_CLASSES)

		// declare templates/using
	template<typename CONSTNESS_OPERATOR>
	class TileCollisionInfoBase;
	using TileCollisionInfo = TileCollisionInfoBase<boost::mpl::identity<boost::mpl::_1>>;
	using TileCollisionConstInfo = TileCollisionInfoBase<boost::add_const<boost::mpl::_1>>;

	template<typename CONSTNESS_OPERATOR>
	class TMCollisionIteratorBase;

	template<typename CONSTNESS_OPERATOR>
	class TMTileCollisionIteratorBase;

	using TMTileCollisionIterator = TMTileCollisionIteratorBase<boost::mpl::identity<boost::mpl::_1>>;
	using TMTileCollisionConstIterator = TMTileCollisionIteratorBase<boost::add_const<boost::mpl::_1>>;


	template<typename T, typename CONSTNESS_OPERATOR>
	class TMObjectCollisionIteratorBase;

	using TMObjectCollisionIterator = TMObjectCollisionIteratorBase<TMObject, boost::mpl::identity<boost::mpl::_1>>;
	using TMTriggerCollisionIterator = TMObjectCollisionIteratorBase<TMTrigger, boost::mpl::identity<boost::mpl::_1>>;
	using TMObjectCollisionConstIterator = TMObjectCollisionIteratorBase<TMObject, boost::add_const<boost::mpl::_1>>;
	using TMTriggerCollisionConstIterator = TMObjectCollisionIteratorBase<TMTrigger, boost::add_const<boost::mpl::_1>>;

	// friendship macro
#define CHAOS_GAPMEPLAY_TM_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_GAMEPLAY_TM_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_GAPMEPLAY_TM_FRIEND_DECL, _, CHAOS_GAMEPLAY_TM_CLASSES)\
	template<typename CONSTNESS_OPERATOR> friend class TileCollisionInfoBase;\
	template<typename CONSTNESS_OPERATOR> friend class TMCollisionIteratorBase;\
	template<typename CONSTNESS_OPERATOR> friend class TMTileCollisionIteratorBase;\
	template<typename T, typename CONSTNESS_OPERATOR> friend class TMObjectCollisionIteratorBase;

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

#include <chaos/Gameplay/TM/TMParticle.h>
#include <chaos/Gameplay/TM/TMObjectReferenceSolver.h>
#include <chaos/Gameplay/TM/TMObject.h>
#include <chaos/Gameplay/TM/TMLevel.h>
#include <chaos/Gameplay/TM/TMLayerInstance.h>
#include <chaos/Gameplay/TM/TMLevelInstance.h>
#include <chaos/Gameplay/TM/TMLayerInstanceIterator.h>
#include <chaos/Gameplay/TM/TMCollision.h>
#include <chaos/Gameplay/TM/TMCollisionComputer.h>
#include <chaos/Gameplay/TM/TMParticlePopulator.h>
#include <chaos/Gameplay/TM/TMTools.h>
