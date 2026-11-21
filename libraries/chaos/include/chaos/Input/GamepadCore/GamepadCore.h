namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	// all classes in this file
#define CHAOS_GAMEPAD_CLASSES \
(PhysicalGamepad) \
(Gamepad) \
(GamepadCallbacks) \
(GamepadManager) \
(ForceFeedbackMotorValues) \
(ForceFeedbackEffect) \
(DefaultForceFeedbackEffect)\
(GamepadPollInterface)

// forward declaration
#define CHAOS_GAMEPAD_FORWARD_DECL(r, data, elem) class elem;

// friendship macro
#define CHAOS_GAMEPAD_FRIEND_DECL(r, data, elem) friend class elem;
#define CHAOS_GAMEPAD_ALL_FRIENDS BOOST_PP_SEQ_FOR_EACH(CHAOS_GAMEPAD_FRIEND_DECL, _, CHAOS_GAMEPAD_CLASSES)

BOOST_PP_SEQ_FOR_EACH(CHAOS_GAMEPAD_FORWARD_DECL, _, CHAOS_GAMEPAD_CLASSES);



#endif

}; // namespace chaos

#include "chaos/Input/GamepadCore/GamepadInputFilterSettings.h"
#include "chaos/Input/GamepadCore/GamepadCallbacks.h"
#include "chaos/Input/GamepadCore/PhysicalGamepad.h"
#include "chaos/Input/GamepadCore/ForceFeedbackEffect.h"
#include "chaos/Input/GamepadCore/DefaultForceFeedbackEffect.h"
#include "chaos/Input/GamepadCore/GamepadPollInterface.h"
#include "chaos/Input/GamepadCore/Gamepad.h"
#include "chaos/Input/GamepadCore/GamepadManager.h"
