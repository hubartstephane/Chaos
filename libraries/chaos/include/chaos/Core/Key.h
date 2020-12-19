#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class Key;

	enum class KeyType;
	enum class GamepadAxis;
	enum class GamepadStick;
	enum class GamepadButton;
	enum class KeyboardButton;
	enum class MouseButton;

}; // namespace chaos

#else 

namespace chaos
{
#define CHAOS_KEYDEF(KEY) KEY = GLFW_KEY_##KEY

	/**
	 * KeyboardButton : A keyboard key
	 */
	enum class KeyboardButton : int
	{
		CHAOS_KEYDEF(UNKNOWN),
		CHAOS_KEYDEF(SPACE),
		CHAOS_KEYDEF(APOSTROPHE),
		CHAOS_KEYDEF(COMMA),
		CHAOS_KEYDEF(MINUS),
		CHAOS_KEYDEF(PERIOD),
		CHAOS_KEYDEF(SLASH),
		NUM_0 = GLFW_KEY_0,
		NUM_1 = GLFW_KEY_1,
		NUM_2 = GLFW_KEY_2,
		NUM_3 = GLFW_KEY_3,
		NUM_4 = GLFW_KEY_4,
		NUM_5 = GLFW_KEY_5,
		NUM_6 = GLFW_KEY_6,
		NUM_7 = GLFW_KEY_7,
		NUM_8 = GLFW_KEY_8,
		NUM_9 = GLFW_KEY_9,
		CHAOS_KEYDEF(SEMICOLON),
		CHAOS_KEYDEF(EQUAL),
		CHAOS_KEYDEF(A),
		CHAOS_KEYDEF(B),
		CHAOS_KEYDEF(C),
		CHAOS_KEYDEF(D),
		CHAOS_KEYDEF(E),
		CHAOS_KEYDEF(F),
		CHAOS_KEYDEF(G),
		CHAOS_KEYDEF(H),
		CHAOS_KEYDEF(I),
		CHAOS_KEYDEF(J),
		CHAOS_KEYDEF(K),
		CHAOS_KEYDEF(L),
		CHAOS_KEYDEF(M),
		CHAOS_KEYDEF(N),
		CHAOS_KEYDEF(O),
		CHAOS_KEYDEF(P),
		CHAOS_KEYDEF(Q),
		CHAOS_KEYDEF(R),
		CHAOS_KEYDEF(S),
		CHAOS_KEYDEF(T),
		CHAOS_KEYDEF(U),
		CHAOS_KEYDEF(V),
		CHAOS_KEYDEF(W),
		CHAOS_KEYDEF(X),
		CHAOS_KEYDEF(Y),
		CHAOS_KEYDEF(Z),
		CHAOS_KEYDEF(LEFT_BRACKET),
		CHAOS_KEYDEF(BACKSLASH),
		CHAOS_KEYDEF(RIGHT_BRACKET),
		CHAOS_KEYDEF(GRAVE_ACCENT),
		CHAOS_KEYDEF(WORLD_1),
		CHAOS_KEYDEF(WORLD_2),
		CHAOS_KEYDEF(ESCAPE),
		CHAOS_KEYDEF(ENTER),
		CHAOS_KEYDEF(TAB),
		CHAOS_KEYDEF(BACKSPACE),
		CHAOS_KEYDEF(INSERT),
		CHAOS_KEYDEF(DELETE),
		CHAOS_KEYDEF(RIGHT),
		CHAOS_KEYDEF(LEFT),
		CHAOS_KEYDEF(DOWN),
		CHAOS_KEYDEF(UP),
		CHAOS_KEYDEF(PAGE_UP),
		CHAOS_KEYDEF(PAGE_DOWN),
		CHAOS_KEYDEF(HOME),
		CHAOS_KEYDEF(END),
		CHAOS_KEYDEF(CAPS_LOCK),
		CHAOS_KEYDEF(SCROLL_LOCK),
		CHAOS_KEYDEF(NUM_LOCK),
		CHAOS_KEYDEF(PRINT_SCREEN),
		CHAOS_KEYDEF(PAUSE),
		CHAOS_KEYDEF(F1),
		CHAOS_KEYDEF(F2),
		CHAOS_KEYDEF(F3),
		CHAOS_KEYDEF(F4),
		CHAOS_KEYDEF(F5),
		CHAOS_KEYDEF(F6),
		CHAOS_KEYDEF(F7),
		CHAOS_KEYDEF(F8),
		CHAOS_KEYDEF(F9),
		CHAOS_KEYDEF(F10),
		CHAOS_KEYDEF(F11),
		CHAOS_KEYDEF(F12),
		CHAOS_KEYDEF(F13),
		CHAOS_KEYDEF(F14),
		CHAOS_KEYDEF(F15),
		CHAOS_KEYDEF(F16),
		CHAOS_KEYDEF(F17),
		CHAOS_KEYDEF(F18),
		CHAOS_KEYDEF(F19),
		CHAOS_KEYDEF(F20),
		CHAOS_KEYDEF(F21),
		CHAOS_KEYDEF(F22),
		CHAOS_KEYDEF(F23),
		CHAOS_KEYDEF(F24),
		CHAOS_KEYDEF(F25),
		CHAOS_KEYDEF(KP_0),
		CHAOS_KEYDEF(KP_1),
		CHAOS_KEYDEF(KP_2),
		CHAOS_KEYDEF(KP_3),
		CHAOS_KEYDEF(KP_4),
		CHAOS_KEYDEF(KP_5),
		CHAOS_KEYDEF(KP_6),
		CHAOS_KEYDEF(KP_7),
		CHAOS_KEYDEF(KP_8),
		CHAOS_KEYDEF(KP_9),
		CHAOS_KEYDEF(KP_DECIMAL),
		CHAOS_KEYDEF(KP_DIVIDE),
		CHAOS_KEYDEF(KP_MULTIPLY),
		CHAOS_KEYDEF(KP_SUBTRACT),
		CHAOS_KEYDEF(KP_ADD),
		CHAOS_KEYDEF(KP_ENTER),
		CHAOS_KEYDEF(KP_EQUAL),
		CHAOS_KEYDEF(LEFT_SHIFT),
		CHAOS_KEYDEF(LEFT_CONTROL),
		CHAOS_KEYDEF(LEFT_ALT),
		CHAOS_KEYDEF(LEFT_SUPER),
		CHAOS_KEYDEF(RIGHT_SHIFT),
		CHAOS_KEYDEF(RIGHT_CONTROL),
		CHAOS_KEYDEF(RIGHT_ALT),
		CHAOS_KEYDEF(RIGHT_SUPER),
		CHAOS_KEYDEF(MENU)
	};
#undef CHAOS_KEYDEF

	/**
	 * MouseButton
	 */
	enum class MouseButton : int
	{
		UNKNOWN  = -1,
		BUTTON_1 = GLFW_MOUSE_BUTTON_1,
		BUTTON_2 = GLFW_MOUSE_BUTTON_2,
		BUTTON_3 = GLFW_MOUSE_BUTTON_3,
		BUTTON_4 = GLFW_MOUSE_BUTTON_4,
		BUTTON_5 = GLFW_MOUSE_BUTTON_5,
		BUTTON_6 = GLFW_MOUSE_BUTTON_6,
		BUTTON_7 = GLFW_MOUSE_BUTTON_7,
		BUTTON_8 = GLFW_MOUSE_BUTTON_8
	};


	// XXX: there are important changes in GLFW 3.3 relative to 3.1
	//
	//  -XBOX_RIGHT_AXIS_X and XBOX_RIGHT_AXIS_Y : are bound to other indices.
	//
	//  -in GLFW 3.1, LEFT and RIGHT triggers were bound to the same index
	//                LEFT  giving a value in [-1 .. 0] 
	//                RIGHT giving a value in [0 .. +1] 
	//
	//  -in GLFW 3.3, LEFT are RIGHT TRIGGERS are now bound to different indices 
	//                LEFT  giving a value in [-1 .. +1] 
	//                RIGHT giving a value in [-1 .. +1] 
	//
	//                while they are not bound to the same index, you can check both values

	// shuxxx see glfw3.h => some values seem to differ    GLFW_GAMEPAD_BUTTON_LEFT_THUMB ? etc

	enum class GamepadButton : int
	{
		/** undefined value */
		UNKNOWN = -1,
		/** index in buttons of A for XBOX like pad */
		A = GLFW_GAMEPAD_BUTTON_A,
		/** index in buttons of B for XBOX like pad */
		B = GLFW_GAMEPAD_BUTTON_B,
		/** index in buttons of X for XBOX like pad */
		X = GLFW_GAMEPAD_BUTTON_X,
		/** index in buttons of Y for XBOX like pad */
		Y = GLFW_GAMEPAD_BUTTON_Y,
		/** index in buttons of LEFT for XBOX like pad (the one behind the LEFT TRIGGER) */
		LEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
		/** index in buttons of RIGHT for XBOX like pad (the one behind the RIGHT TRIGGER) */
		RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
		/** index in buttons of SELECT for XBOX like pad */
		SELECT = GLFW_GAMEPAD_BUTTON_BACK,
		/** index in buttons of START for XBOX like pad */
		START = GLFW_GAMEPAD_BUTTON_START,
		/** index in buttons of SELECT for XBOX like pad */
		GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE,
		/** index in buttons of LEFT-STICK-CLICKED for XBOX like pad */
		LEFT_THUMB = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
		/** index in buttons of RIGHT-STICK-CLICKED for XBOX like pad */
		RIGHT_THUMB = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
		/** index in buttons of UP for XBOX like pad */
		DPAD_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP,
		/** index in buttons of RIGHT for XBOX like pad */
		DPAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
		/** index in buttons of DOWN for XBOX like pad */
		DPAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
		/** index in buttons of LEFT for XBOX like pad */
		DPAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,

		/** index in buttons of LEFT TRIGGER for XBOX like pad (this is a simulate button, while the physical left trigger is an axis) */
		LEFT_TRIGGER = 101,
		/** index in buttons of RIGHT TRIGGER for XBOX like pad (this is a simulate button, while the physical right trigger is an axis) */
		RIGHT_TRIGGER = 102
	};

	enum class GamepadAxis : int
	{
		/** undefined value */
		UNKNOWN = -1,
		/** index in axis of LEFT X for XBOX like pad */
		LEFT_AXIS_X = GLFW_GAMEPAD_AXIS_LEFT_X,
		/** index in axis of LEFT Y for XBOX like pad */
		LEFT_AXIS_Y = GLFW_GAMEPAD_AXIS_LEFT_Y, // STICLEFT_TRIGGERK DOWN = positive values

		/** index in axis of RIGHT X for XBOX like pad */
		RIGHT_AXIS_X = GLFW_GAMEPAD_AXIS_RIGHT_X,
		/** index in axis of RIGHT Y for XBOX like pad */
		RIGHT_AXIS_Y = GLFW_GAMEPAD_AXIS_RIGHT_Y,  // STICK DOWN = positive values

		/** index in axis for the LEFT trigger for XBOX like pad (beware its value is between [-1 .. +1]) */
		LEFT_TRIGGER = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
		/** index in axis for the RIGHT trigger for XBOX like pad (beware its value is between [-1 .. +1]) */
		RIGHT_TRIGGER = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
	};

	enum class GamepadStick : int
	{
		/** undefined value */
		UNKNOWN = -1,
		/** returns the direction of left stick */
		LEFT_STICK = 0,
		/** returns the direction of right stick */
		RIGHT_STICK = 1
	};

	enum class KeyType : int
	{
		/** undefined value */
		UNKNOWN  = -1,
		/** undefined value */
		KEYBOARD = 0,
		/** undefined value */
		MOUSE    = 1,
		/** undefined value */
		GAMEPAD  = 2
	};

	/**
	 * Key : a generic key (gamepad, keyboard or mouse)
	 */
	class Key
	{
	public:

		/** default constructor */
		Key();
		/** copy constructor */
		Key(Key const& src) = default;
		/** key from its name */
		Key(char const* name);
		/** gamepad key constructor */
		Key(GamepadButton button);
		/** keyboard key constructor */
		Key(KeyboardButton button);
		/** mouse key constructor */
		Key(MouseButton button);

		/** get a key name  */
		char const* GetName() const;
		/** check whether the key is valid or not */
		bool IsValid() const;
		/** gets the type */
		KeyType GetType() const;

		/** comparaison operator */
		bool operator == (Key const& src) const;
		/** comparaison operator */
		bool operator != (Key const& src) const;

		/** get the keyboard value for the key (UNKNOWN if it is not a keyboard key) */
		KeyboardButton GetKeyboardButton() const;
		/** get the gamepad value for the key (UNKNOWN if it is not a gamepad key) */
		GamepadButton GetGamepadButton() const;
		/** get the mouse value for the key (UNKNOWN if it is not a mouse key) */
		MouseButton GetMouseButton() const;

		/** get the raw value of the concerned union member */
		int GetRawValue() const;

	protected:

		/** the type of key */
		KeyType type = KeyType::UNKNOWN;
		/** the value of the key */
		union
		{
			KeyboardButton keyboard_button;
			MouseButton mouse_button;
			GamepadButton gamepad_button;
		};
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION

