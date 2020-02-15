#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/InputMode.h>

namespace chaos
{
	class InputEventReceiver
	{

	public:

		/** destructor */
		virtual ~InputEventReceiver() = default;

		/** change the current input mode */
		void SetInputMode(InputMode new_mode);
		/* get the current input mode */
		InputMode GetInputMode() const { return input_mode; }
		
		/** called whenever the mouse is moved */
		bool OnMouseMove(double x, double y);
		/** called whenever the mouse button is down / up */
		bool OnMouseButton(int button, int action, int modifier);
		/** called whenever the mouse wheel is changed */
		bool OnMouseWheel(double scroll_x, double scroll_y);
		/** called whenever a key is pressed */
		bool OnKeyEvent(int key, int scan_code, int action, int modifier);
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
		virtual bool OnKeyEventImpl(int key, int scan_code, int action, int modifier);
		/** called whenever a char is generated */
		virtual bool OnCharEventImpl(unsigned int c);

		/** the user callback called when current input mode changes */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode);

	protected:

		/** the current input mode */
		InputMode input_mode = InputMode::Keyboard;
	};

}; // namespace chaos

