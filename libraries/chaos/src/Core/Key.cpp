
#include <chaos/Chaos.h>

namespace chaos
{

#define CHAOS_KEYBOARD_DEF(KEY) std::make_pair(KeyboardButton::KEY, #KEY)

	static std::vector<std::pair<Key, char const *>> const key_map =
	{
		// keyboard entries
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
CHAOS_KEYBOARD_DEF(MENU),
// mouse entries
std::make_pair(MouseButton::BUTTON_1, "MOUSE_BUTTON_1"),
std::make_pair(MouseButton::BUTTON_2, "MOUSE_BUTTON_2"),
std::make_pair(MouseButton::BUTTON_3, "MOUSE_BUTTON_3"),
std::make_pair(MouseButton::BUTTON_4, "MOUSE_BUTTON_4"),
std::make_pair(MouseButton::BUTTON_5, "MOUSE_BUTTON_5"),
std::make_pair(MouseButton::BUTTON_6, "MOUSE_BUTTON_6"),
std::make_pair(MouseButton::BUTTON_7, "MOUSE_BUTTON_7"),
std::make_pair(MouseButton::BUTTON_8, "MOUSE_BUTTON_8"),
// gamepad button
std::make_pair(GamepadButton::A, "GAMEPAD_A"),
std::make_pair(GamepadButton::B, "GAMEPAD_B"),
std::make_pair(GamepadButton::X, "GAMEPAD_X"),
std::make_pair(GamepadButton::Y, "GAMEPAD_Y"),
std::make_pair(GamepadButton::LEFT_BUMPER, "GAMEPAD_LEFT_BUMPER"),
std::make_pair(GamepadButton::RIGHT_BUMPER, "GAMEPAD_RIGHT_BUMPER"),
std::make_pair(GamepadButton::SELECT, "GAMEPAD_SELECT"),
std::make_pair(GamepadButton::START, "GAMEPAD_START"),
std::make_pair(GamepadButton::SELECT, "GAMEPAD_SELECT"),
std::make_pair(GamepadButton::GUIDE, "GAMEPAD_GUIDE"),
std::make_pair(GamepadButton::LEFT_THUMB, "GAMEPAD_LEFT_THUMB"),
std::make_pair(GamepadButton::RIGHT_THUMB, "GAMEPAD_RIGHT_THUMB"),
std::make_pair(GamepadButton::DPAD_UP, "GAMEPAD_DPAD_UP"),
std::make_pair(GamepadButton::DPAD_RIGHT, "GAMEPAD_DPAD_RIGHT"),
std::make_pair(GamepadButton::DPAD_DOWN, "GAMEPAD_DPAD_DOWN"),
std::make_pair(GamepadButton::DPAD_LEFT, "GAMEPAD_DPAD_LEFT"),
std::make_pair(GamepadButton::LEFT_TRIGGER, "GAMEPAD_LEFT_TRIGGER"),
std::make_pair(GamepadButton::RIGHT_TRIGGER, "GAMEPAD_RIGHT_TRIGGER")
	};
#undef CHAOS_KEYBOARD_DEF
	
	Key::Key(char const* name) :
		type(KeyType::UNKNOWN),
		gamepad_button(GamepadButton::UNKNOWN) // due to union aliasing, this works fine
	{
		// early exit
		if (StringTools::IsEmpty(name))
			return;
		// search by name
		for (auto const& entry : key_map)
		{
			if (StringTools::Stricmp(name, entry.second) == 0)
			{
				*this = entry.first;
				break;
			}				
		}			
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

	bool Key::operator != (Key const& src) const
	{
		return !operator == (src);
	}

	char const* Key::GetName() const
	{
		for (auto const& entry : key_map)
			if (*this == entry.first)
				return entry.second;
		return nullptr;
	}
	
	bool Key::IsValid() const
	{
		if (type == KeyType::UNKNOWN)
			return false;
		if (keyboard_button == KeyboardButton::UNKNOWN) // this works for all types due to union aliasing 
			return false;
		return false;
	}

	KeyType Key::GetType() const
	{
		if (keyboard_button == KeyboardButton::UNKNOWN) // this works for all types due to union aliasing 
			return KeyType::UNKNOWN;
		return type;
	}

}; // namespace chaos
