#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	// -------------------------------------------------------------------
	// MouseButton implementation
	// -------------------------------------------------------------------

	static EnumMetaData<MouseButton> const MouseButton_metadata =
	{
		{ MouseButton::BUTTON_1, "MOUSE_BUTTON_1" },
		{ MouseButton::BUTTON_2, "MOUSE_BUTTON_2" },
		{ MouseButton::BUTTON_3, "MOUSE_BUTTON_3" },
		{ MouseButton::BUTTON_4, "MOUSE_BUTTON_4" },
		{ MouseButton::BUTTON_5, "MOUSE_BUTTON_5" },
		{ MouseButton::BUTTON_6, "MOUSE_BUTTON_6" },
		{ MouseButton::BUTTON_7, "MOUSE_BUTTON_7" },
		{ MouseButton::BUTTON_8, "MOUSE_BUTTON_8" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(MouseButton, &MouseButton_metadata, CHAOS_API);

	// -------------------------------------------------------------------
	// GamepadButton implementation
	// -------------------------------------------------------------------

	static EnumMetaData<GamepadButton> const GamepadButton_metadata =
	{
		{ GamepadButton::A, "GAMEPAD_A" },
		{ GamepadButton::B, "GAMEPAD_B" },
		{ GamepadButton::X, "GAMEPAD_X" },
		{ GamepadButton::Y, "GAMEPAD_Y" },
		{ GamepadButton::LEFT_BUMPER, "GAMEPAD_LEFT_BUMPER" },
		{ GamepadButton::RIGHT_BUMPER, "GAMEPAD_RIGHT_BUMPER" },
		{ GamepadButton::SPECIAL_LEFT, "GAMEPAD_SPECIAL_LEFT" },
		{ GamepadButton::SPECIAL_RIGHT, "GAMEPAD_SPECIAL_RIGHT" },
		{ GamepadButton::GUIDE, "GAMEPAD_GUIDE" },
		{ GamepadButton::LEFT_THUMB, "GAMEPAD_LEFT_THUMB" },
		{ GamepadButton::RIGHT_THUMB, "GAMEPAD_RIGHT_THUMB" },
		{ GamepadButton::DPAD_UP, "GAMEPAD_DPAD_UP" },
		{ GamepadButton::DPAD_RIGHT, "GAMEPAD_DPAD_RIGHT" },
		{ GamepadButton::DPAD_DOWN, "GAMEPAD_DPAD_DOWN" },
		{ GamepadButton::DPAD_LEFT, "GAMEPAD_DPAD_LEFT" },
		{ GamepadButton::LEFT_TRIGGER, "GAMEPAD_LEFT_TRIGGER" },
		{ GamepadButton::RIGHT_TRIGGER, "GAMEPAD_RIGHT_TRIGGER" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(GamepadButton, &GamepadButton_metadata, CHAOS_API);

	// -------------------------------------------------------------------
	// KeyboardButton implementation
	// -------------------------------------------------------------------

	#define CHAOS_KEYBOARD_DEF(KEY) { KeyboardButton::KEY, #KEY }

	static EnumMetaData<KeyboardButton> const KeyboardButton_metadata =
	{
		CHAOS_KEYBOARD_DEF(SPACE),
		CHAOS_KEYBOARD_DEF(APOSTROPHE),
		CHAOS_KEYBOARD_DEF(COMMA),
		CHAOS_KEYBOARD_DEF(MINUS),
		CHAOS_KEYBOARD_DEF(PERIOD),
		CHAOS_KEYBOARD_DEF(SLASH),
		CHAOS_KEYBOARD_DEF(NUM_0),
		CHAOS_KEYBOARD_DEF(NUM_1),
		CHAOS_KEYBOARD_DEF(NUM_2),
		CHAOS_KEYBOARD_DEF(NUM_3),
		CHAOS_KEYBOARD_DEF(NUM_4),
		CHAOS_KEYBOARD_DEF(NUM_5),
		CHAOS_KEYBOARD_DEF(NUM_6),
		CHAOS_KEYBOARD_DEF(NUM_7),
		CHAOS_KEYBOARD_DEF(NUM_8),
		CHAOS_KEYBOARD_DEF(NUM_9),
		CHAOS_KEYBOARD_DEF(SEMICOLON),
		CHAOS_KEYBOARD_DEF(EQUAL),
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
		CHAOS_KEYBOARD_DEF(LEFT_BRACKET),
		CHAOS_KEYBOARD_DEF(BACKSLASH),
		CHAOS_KEYBOARD_DEF(RIGHT_BRACKET),
		CHAOS_KEYBOARD_DEF(GRAVE_ACCENT),
		CHAOS_KEYBOARD_DEF(WORLD_1),
		CHAOS_KEYBOARD_DEF(WORLD_2),
		CHAOS_KEYBOARD_DEF(ESCAPE),
		CHAOS_KEYBOARD_DEF(ENTER),
		CHAOS_KEYBOARD_DEF(TAB),
		CHAOS_KEYBOARD_DEF(BACKSPACE),
		CHAOS_KEYBOARD_DEF(INSERT),
		CHAOS_KEYBOARD_DEF(DELETE),
		CHAOS_KEYBOARD_DEF(RIGHT),
		CHAOS_KEYBOARD_DEF(LEFT),
		CHAOS_KEYBOARD_DEF(DOWN),
		CHAOS_KEYBOARD_DEF(UP),
		CHAOS_KEYBOARD_DEF(PAGE_UP),
		CHAOS_KEYBOARD_DEF(PAGE_DOWN),
		CHAOS_KEYBOARD_DEF(HOME),
		CHAOS_KEYBOARD_DEF(END),
		CHAOS_KEYBOARD_DEF(CAPS_LOCK),
		CHAOS_KEYBOARD_DEF(SCROLL_LOCK),
		CHAOS_KEYBOARD_DEF(NUM_LOCK),
		CHAOS_KEYBOARD_DEF(PRINT_SCREEN),
		CHAOS_KEYBOARD_DEF(PAUSE),
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
		CHAOS_KEYBOARD_DEF(KP_0),
		CHAOS_KEYBOARD_DEF(KP_1),
		CHAOS_KEYBOARD_DEF(KP_2),
		CHAOS_KEYBOARD_DEF(KP_3),
		CHAOS_KEYBOARD_DEF(KP_4),
		CHAOS_KEYBOARD_DEF(KP_5),
		CHAOS_KEYBOARD_DEF(KP_6),
		CHAOS_KEYBOARD_DEF(KP_7),
		CHAOS_KEYBOARD_DEF(KP_8),
		CHAOS_KEYBOARD_DEF(KP_9),
		CHAOS_KEYBOARD_DEF(KP_DECIMAL),
		CHAOS_KEYBOARD_DEF(KP_DIVIDE),
		CHAOS_KEYBOARD_DEF(KP_MULTIPLY),
		CHAOS_KEYBOARD_DEF(KP_SUBTRACT),
		CHAOS_KEYBOARD_DEF(KP_ADD),
		CHAOS_KEYBOARD_DEF(KP_ENTER),
		CHAOS_KEYBOARD_DEF(KP_EQUAL),
		CHAOS_KEYBOARD_DEF(LEFT_SHIFT),
		CHAOS_KEYBOARD_DEF(LEFT_CONTROL),
		CHAOS_KEYBOARD_DEF(LEFT_ALT),
		CHAOS_KEYBOARD_DEF(LEFT_SUPER),
		CHAOS_KEYBOARD_DEF(RIGHT_SHIFT),
		CHAOS_KEYBOARD_DEF(RIGHT_CONTROL),
		CHAOS_KEYBOARD_DEF(RIGHT_ALT),
		CHAOS_KEYBOARD_DEF(RIGHT_SUPER),
		CHAOS_KEYBOARD_DEF(MENU)
	};
#undef CHAOS_KEYBOARD_DEF

	CHAOS_IMPLEMENT_ENUM_METHOD(KeyboardButton, &KeyboardButton_metadata, CHAOS_API);

	// -------------------------------------------------------------------
	// GamepadAxis implementation
	// -------------------------------------------------------------------

	static EnumMetaData<GamepadAxis> const GamepadAxis_metadata =
	{
		{ GamepadAxis::UNKNOWN, "UNKNOWN" },
		{ GamepadAxis::LEFT_AXIS_X, "LEFT_AXIS_X" },
		{ GamepadAxis::LEFT_AXIS_Y, "LEFT_AXIS_Y" },
		{ GamepadAxis::RIGHT_AXIS_X, "RIGHT_AXIS_X" },
		{ GamepadAxis::RIGHT_AXIS_Y, "RIGHT_AXIS_Y" },
		{ GamepadAxis::LEFT_TRIGGER, "LEFT_TRIGGER" },
		{ GamepadAxis::RIGHT_TRIGGER, "RIGHT_TRIGGER" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(GamepadAxis, &GamepadAxis_metadata, CHAOS_API);


	// -------------------------------------------------------------------
	// GamepadStick implementation
	// -------------------------------------------------------------------

	static EnumMetaData<GamepadStick> const GamepadStick_metadata =
	{
		{ GamepadStick::UNKNOWN, "UNKNOWN" },
		{ GamepadStick::LEFT_STICK, "LEFT_STICK" },
		{ GamepadStick::RIGHT_STICK, "RIGHT_STICK" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(GamepadStick, &GamepadStick_metadata, CHAOS_API);

	// -------------------------------------------------------------------
	// KeyType implementation
	// -------------------------------------------------------------------

	static EnumMetaData<KeyType> const KeyType_metadata =
	{
		{ KeyType::UNKNOWN, "UNKNOWN" },
		{ KeyType::KEYBOARD, "KEYBOARD" },
		{ KeyType::MOUSE, "MOUSE" },
		{ KeyType::GAMEPAD, "GAMEPAD" }
	};

	CHAOS_IMPLEMENT_ENUM_METHOD(KeyType, &KeyType_metadata, CHAOS_API);

	// -------------------------------------------------------------------
	// Key implementation
	// -------------------------------------------------------------------

	Key::Key(char const* name) :
		type(KeyType::UNKNOWN),
		gamepad_button(GamepadButton::UNKNOWN) // due to union aliasing, this works fine
	{
		// early exit
		if (StringTools::IsEmpty(name))
			return;

		// search the name in Keyboard's, Gamepad's and Mouse's
		auto search_by_name = [this](KeyType key_type, char const * name, auto & result_enum) -> bool
		{
			if (StringToEnum(name, result_enum))
			{
				type = key_type;
				return true;
			}
			return false;
		};

		if (!search_by_name(KeyType::KEYBOARD, name, keyboard_button))
			if (!search_by_name(KeyType::GAMEPAD, name, gamepad_button))
				search_by_name(KeyType::MOUSE, name, mouse_button);
	}

	Key::Key() :
		type(KeyType::UNKNOWN),
		gamepad_button(GamepadButton::UNKNOWN) // due to union aliasing, this works fine
	{
	}

	Key::Key(GamepadButton button) :
		type(KeyType::GAMEPAD),
		gamepad_button(button)
	{
		if (gamepad_button == GamepadButton::UNKNOWN)
			type = KeyType::UNKNOWN;
	}

	Key::Key(KeyboardButton button) :
		type(KeyType::KEYBOARD),
		keyboard_button(button)
	{
		if (keyboard_button == KeyboardButton::UNKNOWN)
			type = KeyType::UNKNOWN;
	}

	Key::Key(MouseButton button) :
		type(KeyType::MOUSE),
		mouse_button(button)
	{
		if (mouse_button == MouseButton::UNKNOWN)
			type = KeyType::UNKNOWN;
	}

	bool Key::operator == (Key const& src) const
	{
		if (type == src.type)
		{
			if (type == KeyType::UNKNOWN) // ignore other data if type is unknown
				return true;
			return (keyboard_button == src.keyboard_button); // this works for all types due to union aliasing
		}
		return false;
	}

	char const* Key::GetName() const
	{
		switch (type)
		{
		case KeyType::KEYBOARD:
			return EnumToString(keyboard_button);
		case KeyType::GAMEPAD:
			return EnumToString(gamepad_button);
		case KeyType::MOUSE:
			return EnumToString(mouse_button);
		}
		return nullptr;
	}

	bool Key::IsValid() const
	{
		if (type == KeyType::UNKNOWN)
			return false;
		if (keyboard_button == KeyboardButton::UNKNOWN) // this works for all types due to union aliasing
			return false;
		return true;
	}

	KeyType Key::GetType() const
	{
		if (keyboard_button == KeyboardButton::UNKNOWN) // this works for all types due to union aliasing
			return KeyType::UNKNOWN;
		return type;
	}

	KeyboardButton Key::GetKeyboardButton() const
	{
		if (type != KeyType::KEYBOARD)
			return KeyboardButton::UNKNOWN;
		return keyboard_button;
	}

	GamepadButton Key::GetGamepadButton() const
	{
		if (type != KeyType::GAMEPAD)
			return GamepadButton::UNKNOWN;
		return gamepad_button;
	}

	MouseButton Key::GetMouseButton() const
	{
		if (type != KeyType::MOUSE)
			return MouseButton::UNKNOWN;
		return mouse_button;
	}

	int Key::GetRawValue() const
	{
		if (type == KeyType::UNKNOWN)
			return -1;
		return int(keyboard_button); // this works for all types due to union aliasing
	}

}; // namespace chaos
