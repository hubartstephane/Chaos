namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class Key;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

#define CHAOS_KEYDEF(NAME, GLFW_KEY) NAME = KeyboardFirst + GLFW_KEY_##GLFW_KEY

	/**
	 * Key: enumeration to match a key input (whether it is a a keyboard, mouse or gamepad input)
	 */

	enum class Key : int
	{
		Unknown = -1,

		KeyboardFirst = 0,
		CHAOS_KEYDEF(Space, SPACE),
		CHAOS_KEYDEF(Apostrophe, APOSTROPHE),
		CHAOS_KEYDEF(Comma, COMMA),
		CHAOS_KEYDEF(Minus, MINUS),
		CHAOS_KEYDEF(Period, PERIOD),
		CHAOS_KEYDEF(Slash, SLASH),
		Num0 = KeyboardFirst + GLFW_KEY_0,
		Num1 = KeyboardFirst + GLFW_KEY_1,
		Num2 = KeyboardFirst + GLFW_KEY_2,
		Num3 = KeyboardFirst + GLFW_KEY_3,
		Num4 = KeyboardFirst + GLFW_KEY_4,
		Num5 = KeyboardFirst + GLFW_KEY_5,
		Num6 = KeyboardFirst + GLFW_KEY_6,
		Num7 = KeyboardFirst + GLFW_KEY_7,
		Num8 = KeyboardFirst + GLFW_KEY_8,
		Num9 = KeyboardFirst + GLFW_KEY_9,
		CHAOS_KEYDEF(Semicolon, SEMICOLON),
		CHAOS_KEYDEF(Equal, EQUAL),
		CHAOS_KEYDEF(A, A),
		CHAOS_KEYDEF(B, B),
		CHAOS_KEYDEF(C, C),
		CHAOS_KEYDEF(D, D),
		CHAOS_KEYDEF(E, E),
		CHAOS_KEYDEF(F, F),
		CHAOS_KEYDEF(G, G),
		CHAOS_KEYDEF(H, H),
		CHAOS_KEYDEF(I, I),
		CHAOS_KEYDEF(J, J),
		CHAOS_KEYDEF(K, K),
		CHAOS_KEYDEF(L, L),
		CHAOS_KEYDEF(M, M),
		CHAOS_KEYDEF(N, N),
		CHAOS_KEYDEF(O, O),
		CHAOS_KEYDEF(P, P),
		CHAOS_KEYDEF(Q, Q),
		CHAOS_KEYDEF(R, R),
		CHAOS_KEYDEF(S, S),
		CHAOS_KEYDEF(T, T),
		CHAOS_KEYDEF(U, U),
		CHAOS_KEYDEF(V, V),
		CHAOS_KEYDEF(W, W),
		CHAOS_KEYDEF(X, X),
		CHAOS_KEYDEF(Y, Y),
		CHAOS_KEYDEF(Z, Z),
		CHAOS_KEYDEF(LeftBracket, LEFT_BRACKET),
		CHAOS_KEYDEF(Backslash, BACKSLASH),
		CHAOS_KEYDEF(RightBracket, RIGHT_BRACKET),
		CHAOS_KEYDEF(GraveAccent, GRAVE_ACCENT),
		CHAOS_KEYDEF(World1, WORLD_1),
		CHAOS_KEYDEF(World2, WORLD_2),
		CHAOS_KEYDEF(Escape, ESCAPE),
		CHAOS_KEYDEF(Enter, ENTER),
		CHAOS_KEYDEF(Tab, TAB),
		CHAOS_KEYDEF(Backspace, BACKSPACE),
		CHAOS_KEYDEF(Insert, INSERT),
		CHAOS_KEYDEF(Delete, DELETE),
		CHAOS_KEYDEF(Right, RIGHT),
		CHAOS_KEYDEF(Left, LEFT),
		CHAOS_KEYDEF(Down, DOWN),
		CHAOS_KEYDEF(Up, UP),
		CHAOS_KEYDEF(PageUp, PAGE_UP),
		CHAOS_KEYDEF(PageDown, PAGE_DOWN),
		CHAOS_KEYDEF(Home, HOME),
		CHAOS_KEYDEF(End, END),
		CHAOS_KEYDEF(CapsLock, CAPS_LOCK),
		CHAOS_KEYDEF(ScrollLock, SCROLL_LOCK),
		CHAOS_KEYDEF(NumLock, NUM_LOCK),
		CHAOS_KEYDEF(PrintScreen, PRINT_SCREEN),
		CHAOS_KEYDEF(Pause, PAUSE),
		CHAOS_KEYDEF(F1, F1),
		CHAOS_KEYDEF(F2, F2),
		CHAOS_KEYDEF(F3, F3),
		CHAOS_KEYDEF(F4, F4),
		CHAOS_KEYDEF(F5, F5),
		CHAOS_KEYDEF(F6, F6),
		CHAOS_KEYDEF(F7, F7),
		CHAOS_KEYDEF(F8, F8),
		CHAOS_KEYDEF(F9, F9),
		CHAOS_KEYDEF(F10, F10),
		CHAOS_KEYDEF(F11, F11),
		CHAOS_KEYDEF(F12, F12),
		CHAOS_KEYDEF(F13, F13),
		CHAOS_KEYDEF(F14, F14),
		CHAOS_KEYDEF(F15, F15),
		CHAOS_KEYDEF(F16, F16),
		CHAOS_KEYDEF(F17, F17),
		CHAOS_KEYDEF(F18, F18),
		CHAOS_KEYDEF(F19, F19),
		CHAOS_KEYDEF(F20, F20),
		CHAOS_KEYDEF(F21, F21),
		CHAOS_KEYDEF(F22, F22),
		CHAOS_KEYDEF(F23, F23),
		CHAOS_KEYDEF(F24, F24),
		CHAOS_KEYDEF(F25, F25),
		CHAOS_KEYDEF(Keypad0, KP_0),
		CHAOS_KEYDEF(Keypad1, KP_1),
		CHAOS_KEYDEF(Keypad2, KP_2),
		CHAOS_KEYDEF(Keypad3, KP_3),
		CHAOS_KEYDEF(Keypad4, KP_4),
		CHAOS_KEYDEF(Keypad5, KP_5),
		CHAOS_KEYDEF(Keypad6, KP_6),
		CHAOS_KEYDEF(Keypad7, KP_7),
		CHAOS_KEYDEF(Keypad8, KP_8),
		CHAOS_KEYDEF(Keypad9, KP_9),
		CHAOS_KEYDEF(KeypadDecimal, KP_DECIMAL),
		CHAOS_KEYDEF(KeypadDivide, KP_DIVIDE),
		CHAOS_KEYDEF(KeypadMultiply, KP_MULTIPLY),
		CHAOS_KEYDEF(KeypadSubtract, KP_SUBTRACT),
		CHAOS_KEYDEF(KeypadAdd, KP_ADD),
		CHAOS_KEYDEF(KeypadEnter, KP_ENTER),
		CHAOS_KEYDEF(KeypadEqual, KP_EQUAL),
		CHAOS_KEYDEF(LeftShift, LEFT_SHIFT),
		CHAOS_KEYDEF(LeftControl, LEFT_CONTROL),
		CHAOS_KEYDEF(LeftAlt, LEFT_ALT),
		CHAOS_KEYDEF(LeftSuper, LEFT_SUPER),
		CHAOS_KEYDEF(RightShift, RIGHT_SHIFT),
		CHAOS_KEYDEF(RightControl, RIGHT_CONTROL),
		CHAOS_KEYDEF(RightAlt, RIGHT_ALT),
		CHAOS_KEYDEF(RightSuper, RIGHT_SUPER),
		CHAOS_KEYDEF(Menu, MENU),
		KeyboardLast = GLFW_KEY_MENU,

		MouseFirst = 1000,
		MouseButton1 = MouseFirst + GLFW_MOUSE_BUTTON_1,
		MouseButton2 = MouseFirst + GLFW_MOUSE_BUTTON_2,
		MouseButton3 = MouseFirst + GLFW_MOUSE_BUTTON_3,
		MouseButton4 = MouseFirst + GLFW_MOUSE_BUTTON_4,
		MouseButton5 = MouseFirst + GLFW_MOUSE_BUTTON_5,
		MouseButton6 = MouseFirst + GLFW_MOUSE_BUTTON_6,
		MouseButton7 = MouseFirst + GLFW_MOUSE_BUTTON_7,
		MouseButton8 = MouseFirst + GLFW_MOUSE_BUTTON_8,
		MouseLast = MouseButton8,

		GamepadFirst = 2000,
		GamepadA            = GamepadFirst + GLFW_GAMEPAD_BUTTON_A,
		GamepadB            = GamepadFirst + GLFW_GAMEPAD_BUTTON_B,
		GamepadX            = GamepadFirst + GLFW_GAMEPAD_BUTTON_X,
		GamepadY            = GamepadFirst + GLFW_GAMEPAD_BUTTON_Y,
		GamepadLeftBumper   = GamepadFirst + GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
		GamepadRightBumper  = GamepadFirst + GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
		GamepadSpecialLeft  = GamepadFirst + GLFW_GAMEPAD_BUTTON_BACK,
		GamepadSpecialRight = GamepadFirst + GLFW_GAMEPAD_BUTTON_START,
		GamepadGuide        = GamepadFirst + GLFW_GAMEPAD_BUTTON_GUIDE,
		GamepadLeftThumb    = GamepadFirst + GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
		GamepadRightThumb   = GamepadFirst + GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
		GamepadDpadUp       = GamepadFirst + GLFW_GAMEPAD_BUTTON_DPAD_UP,
		GamepadDpadRight    = GamepadFirst + GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
		GamepadDpadDown     = GamepadFirst + GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
		GamepadDpadLeft     = GamepadFirst + GLFW_GAMEPAD_BUTTON_DPAD_LEFT,

		GamepadLeftTrigger  = GamepadFirst + 15, // theses are simulated inputs (Left/Right Trigger is an axis for XBOX gamepad point of view)
		GamepadRightTrigger = GamepadFirst + 16,
		GamepadLast = GamepadRightTrigger
	};

	CHAOS_DECLARE_ENUM_METHOD(Key, CHAOS_API);

#undef CHAOS_KEYDEF

	CHAOS_API bool IsKeyboardInput(Key in_key);

	CHAOS_API bool IsMouseInput(Key in_key);

	CHAOS_API bool IsGamepadInput(Key in_key);

	CHAOS_API Key GetKeyFromName(char const * in_name);

#endif

}; // namespace chaos