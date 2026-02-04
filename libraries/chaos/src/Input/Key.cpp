#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	#define CHAOS_KEYBOARD_DEF(KEY) { Key::KEY, #KEY }

	static EnumMetaData<Key> const Key_metadata =
	{
		CHAOS_KEYBOARD_DEF(Unknown),

		CHAOS_KEYBOARD_DEF(Space),
		CHAOS_KEYBOARD_DEF(Apostrophe),
		CHAOS_KEYBOARD_DEF(Comma),
		CHAOS_KEYBOARD_DEF(Minus),
		CHAOS_KEYBOARD_DEF(Period),
		CHAOS_KEYBOARD_DEF(Slash),
		CHAOS_KEYBOARD_DEF(Num0),
		CHAOS_KEYBOARD_DEF(Num1),
		CHAOS_KEYBOARD_DEF(Num2),
		CHAOS_KEYBOARD_DEF(Num3),
		CHAOS_KEYBOARD_DEF(Num4),
		CHAOS_KEYBOARD_DEF(Num5),
		CHAOS_KEYBOARD_DEF(Num6),
		CHAOS_KEYBOARD_DEF(Num7),
		CHAOS_KEYBOARD_DEF(Num8),
		CHAOS_KEYBOARD_DEF(Num9),
		CHAOS_KEYBOARD_DEF(Semicolon),
		CHAOS_KEYBOARD_DEF(Equal),
		CHAOS_KEYBOARD_DEF(A),
		CHAOS_KEYBOARD_DEF(B),
		CHAOS_KEYBOARD_DEF(C),
		CHAOS_KEYBOARD_DEF(D),
		CHAOS_KEYBOARD_DEF(E),
		CHAOS_KEYBOARD_DEF(F),
		CHAOS_KEYBOARD_DEF(G),
		CHAOS_KEYBOARD_DEF(H),
		CHAOS_KEYBOARD_DEF(I),
		CHAOS_KEYBOARD_DEF(J),
		CHAOS_KEYBOARD_DEF(K),
		CHAOS_KEYBOARD_DEF(L),
		CHAOS_KEYBOARD_DEF(M),
		CHAOS_KEYBOARD_DEF(N),
		CHAOS_KEYBOARD_DEF(O),
		CHAOS_KEYBOARD_DEF(P),
		CHAOS_KEYBOARD_DEF(Q),
		CHAOS_KEYBOARD_DEF(R),
		CHAOS_KEYBOARD_DEF(S),
		CHAOS_KEYBOARD_DEF(T),
		CHAOS_KEYBOARD_DEF(U),
		CHAOS_KEYBOARD_DEF(V),
		CHAOS_KEYBOARD_DEF(W),
		CHAOS_KEYBOARD_DEF(X),
		CHAOS_KEYBOARD_DEF(Y),
		CHAOS_KEYBOARD_DEF(Z),
		CHAOS_KEYBOARD_DEF(LeftBracket),
		CHAOS_KEYBOARD_DEF(Backslash),
		CHAOS_KEYBOARD_DEF(RightBracket),
		CHAOS_KEYBOARD_DEF(GraveAccent),
		CHAOS_KEYBOARD_DEF(World1),
		CHAOS_KEYBOARD_DEF(World2),
		CHAOS_KEYBOARD_DEF(Escape),
		CHAOS_KEYBOARD_DEF(Enter),
		CHAOS_KEYBOARD_DEF(Tab),
		CHAOS_KEYBOARD_DEF(Backspace),
		CHAOS_KEYBOARD_DEF(Insert),
		CHAOS_KEYBOARD_DEF(Delete),
		CHAOS_KEYBOARD_DEF(Right),
		CHAOS_KEYBOARD_DEF(Left),
		CHAOS_KEYBOARD_DEF(Down),
		CHAOS_KEYBOARD_DEF(Up),
		CHAOS_KEYBOARD_DEF(PageUp),
		CHAOS_KEYBOARD_DEF(PageDown),
		CHAOS_KEYBOARD_DEF(Home),
		CHAOS_KEYBOARD_DEF(End),
		CHAOS_KEYBOARD_DEF(CapsLock),
		CHAOS_KEYBOARD_DEF(ScrollLock),
		CHAOS_KEYBOARD_DEF(NumLock),
		CHAOS_KEYBOARD_DEF(PrintScreen),
		CHAOS_KEYBOARD_DEF(Pause),
		CHAOS_KEYBOARD_DEF(F1),
		CHAOS_KEYBOARD_DEF(F2),
		CHAOS_KEYBOARD_DEF(F3),
		CHAOS_KEYBOARD_DEF(F4),
		CHAOS_KEYBOARD_DEF(F5),
		CHAOS_KEYBOARD_DEF(F6),
		CHAOS_KEYBOARD_DEF(F7),
		CHAOS_KEYBOARD_DEF(F8),
		CHAOS_KEYBOARD_DEF(F9),
		CHAOS_KEYBOARD_DEF(F10),
		CHAOS_KEYBOARD_DEF(F11),
		CHAOS_KEYBOARD_DEF(F12),
		CHAOS_KEYBOARD_DEF(F13),
		CHAOS_KEYBOARD_DEF(F14),
		CHAOS_KEYBOARD_DEF(F15),
		CHAOS_KEYBOARD_DEF(F16),
		CHAOS_KEYBOARD_DEF(F17),
		CHAOS_KEYBOARD_DEF(F18),
		CHAOS_KEYBOARD_DEF(F19),
		CHAOS_KEYBOARD_DEF(F20),
		CHAOS_KEYBOARD_DEF(F21),
		CHAOS_KEYBOARD_DEF(F22),
		CHAOS_KEYBOARD_DEF(F23),
		CHAOS_KEYBOARD_DEF(F24),
		CHAOS_KEYBOARD_DEF(F25),
		CHAOS_KEYBOARD_DEF(Keypad0),
		CHAOS_KEYBOARD_DEF(Keypad1),
		CHAOS_KEYBOARD_DEF(Keypad2),
		CHAOS_KEYBOARD_DEF(Keypad3),
		CHAOS_KEYBOARD_DEF(Keypad4),
		CHAOS_KEYBOARD_DEF(Keypad5),
		CHAOS_KEYBOARD_DEF(Keypad6),
		CHAOS_KEYBOARD_DEF(Keypad7),
		CHAOS_KEYBOARD_DEF(Keypad8),
		CHAOS_KEYBOARD_DEF(Keypad9),
		CHAOS_KEYBOARD_DEF(KeypadDecimal),
		CHAOS_KEYBOARD_DEF(KeypadDivide),
		CHAOS_KEYBOARD_DEF(KeypadMultiply),
		CHAOS_KEYBOARD_DEF(KeypadSubtract),
		CHAOS_KEYBOARD_DEF(KeypadAdd),
		CHAOS_KEYBOARD_DEF(KeypadEnter),
		CHAOS_KEYBOARD_DEF(KeypadEqual),
		CHAOS_KEYBOARD_DEF(LeftShift),
		CHAOS_KEYBOARD_DEF(LeftControl),
		CHAOS_KEYBOARD_DEF(LeftAlt),
		CHAOS_KEYBOARD_DEF(LeftSuper),
		CHAOS_KEYBOARD_DEF(RightShift),
		CHAOS_KEYBOARD_DEF(RightControl),
		CHAOS_KEYBOARD_DEF(RightAlt),
		CHAOS_KEYBOARD_DEF(RightSuper),
		CHAOS_KEYBOARD_DEF(Menu),

		{ Key::MouseButton1, "MOUSE_BUTTON_1" },
		{ Key::MouseButton2, "MOUSE_BUTTON_2" },
		{ Key::MouseButton3, "MOUSE_BUTTON_3" },
		{ Key::MouseButton4, "MOUSE_BUTTON_4" },
		{ Key::MouseButton5, "MOUSE_BUTTON_5" },
		{ Key::MouseButton6, "MOUSE_BUTTON_6" },
		{ Key::MouseButton7, "MOUSE_BUTTON_7" },
		{ Key::MouseButton8, "MOUSE_BUTTON_8" },

		{ Key::GamepadA, "GAMEPAD_A" },
		{ Key::GamepadB, "GAMEPAD_B" },
		{ Key::GamepadX, "GAMEPAD_X" },
		{ Key::GamepadY, "GAMEPAD_Y" },
		{ Key::GamepadLeftBumper, "GAMEPAD_LEFT_BUMPER" },
		{ Key::GamepadRightBumper, "GAMEPAD_RIGHT_BUMPER" },
		{ Key::GamepadSpecialLeft, "GAMEPAD_SPECIAL_LEFT" },
		{ Key::GamepadSpecialRight, "GAMEPAD_SPECIAL_RIGHT" },
		{ Key::GamepadGuide, "GAMEPAD_GUIDE" },
		{ Key::GamepadLeftThumb, "GAMEPAD_LEFT_THUMB" },
		{ Key::GamepadRightThumb, "GAMEPAD_RIGHT_THUMB" },
		{ Key::GamepadDpadUp, "GAMEPAD_DPAD_UP" },
		{ Key::GamepadDpadRight, "GAMEPAD_DPAD_RIGHT" },
		{ Key::GamepadDpadDown, "GAMEPAD_DPAD_DOWN" },
		{ Key::GamepadDpadLeft, "GAMEPAD_DPAD_LEFT" },
		{ Key::GamepadLeftTrigger, "GAMEPAD_LEFT_TRIGGER" },
		{ Key::GamepadRightTrigger, "GAMEPAD_RIGHT_TRIGGER" }
	};
#undef CHAOS_KEYBOARD_DEF

	CHAOS_IMPLEMENT_ENUM_METHOD(Key, &Key_metadata, CHAOS_API);

	bool IsKeyboardInput(Key in_key)
	{
		return (in_key >= Key::KeyboardFirst) && (in_key <= Key::KeyboardLast);
	}

	bool IsMouseInput(Key in_key)
	{
		return (in_key >= Key::MouseFirst) && (in_key <= Key::MouseLast);
	}

	bool IsGamepadInput(Key in_key)
	{
		return (in_key >= Key::GamepadFirst) && (in_key <= Key::GamepadLast);
	}

	Key GetKeyFromName(char const* in_name)
	{
		Key result = Key::Unknown;
		if (StringToEnum(in_name, result))
			return result;
		return Key::Unknown;
	}

}; // namespace chaos
