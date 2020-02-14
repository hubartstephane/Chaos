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
		
		/** the user callback called when current input mode changes */
		virtual void OnInputModeChanged(InputMode new_mode, InputMode old_mode);

		/** called whenever the mouse is moved */
		virtual bool OnMouseMove(double x, double y);
		/** called whenever the mouse button is down / up */
		virtual bool OnMouseButton(int button, int action, int modifier);
		/** called whenever the mouse wheel is changed */
		virtual bool OnMouseWheel(double scroll_x, double scroll_y);
		/** called whenever a key is pressed */
		virtual bool OnKeyEvent(int key, int scan_code, int action, int modifier);
		/** called whenever a char is generated */
		virtual bool OnCharEvent(unsigned int c);

	protected:

		/** the current input mode */
		InputMode input_mode = InputMode::Keyboard;
	};

}; // namespace chaos

