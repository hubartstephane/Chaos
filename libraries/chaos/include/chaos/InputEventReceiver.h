#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	class InputEventReceiver
	{

	public:

		/** destructor */
		virtual ~InputEventReceiver() = default;

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
	};

}; // namespace chaos

