#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/InputMode.h>

namespace chaos
{
	class KeyEvent
	{
	public:

		/** check whether this is a key press event */
		bool IsKeyPressed(int check_key, int check_modifier = 0) const;
		/** check whether this is a key release event */
		bool IsKeyReleased(int check_key, int check_modifier = 0) const;

	public:

		/** the key */
		int key = 0;
		/** the scan code */
		int scan_code = 0;
		/** pressed or release */
		int action = 0;
		/** some special key modifiers like shift */
		int modifier = 0;
	};

	class InputEventReceiver
	{

	public:

		/** destructor */
		virtual ~InputEventReceiver() = default;

		/** change the current input mode */
		void SetInputMode(InputMode new_mode);
		/* get the current input mode */
		InputMode GetInputMode() const { return input_mode; }
		
		/** check whether a key event correspond to given request. Change input mode to keyboard if true */
		bool CheckKeyPressed(KeyEvent const& event, int check_key, int check_modifier = 0);
		/** check whether a key is pressed given request. Change input mode to keyboard if true */
		bool CheckKeyPressed(int check_key);



		/** Get the window (usefull for Key function) */
		virtual GLFWwindow* GetGLFWWindow() const { return nullptr; }

		/** called whenever the mouse is moved */
		bool OnMouseMove(double x, double y);
		/** called whenever the mouse button is down / up */
		bool OnMouseButton(int button, int action, int modifier);
		/** called whenever the mouse wheel is changed */
		bool OnMouseWheel(double scroll_x, double scroll_y);
		/** called whenever a key is pressed */
		bool OnKeyEvent(KeyEvent const& event);
		/** called whenever a char is generated */
		bool OnCharEvent(unsigned int c);

	protected:

		/** called whenever the mouse is moved */
		virtual bool OnMouseMoveImpl(double x, double y);
		/** called whenever the mouse button is down / up */
		virtual bool OnMouseButtonImpl(int button, int action, int modifier);
		/** called whenever the mouse wheel is changed */
		virtual bool OnMouseWheelImpl(double scroll_x, double scroll_y);
		/** called whenever a key is pressed */
		virtual bool OnKeyEventImpl(KeyEvent const& event);
		/** called whenever a char is generated */
		virtual bool OnCharEventImpl(unsigned int c);

		/** the user callback called when current input mode changes */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode);

	protected:

		/** the current input mode */
		InputMode input_mode = InputMode::KEYBOARD;
	};

}; // namespace chaos

