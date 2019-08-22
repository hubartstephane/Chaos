
#include <chaos/InputEventReceiver.h>

namespace chaos
{
	bool InputEventReceiver::OnMouseMove(double x, double y) 
	{ 
		return false; 
	}

	bool InputEventReceiver::OnMouseButton(int button, int action, int modifier) 
	{ 
		return false; 
	}

	bool InputEventReceiver::OnMouseWheel(double scroll_x, double scroll_y) 
	{ 
		return false; 
	}

	bool InputEventReceiver::OnKeyEvent(int key, int scan_code, int action, int modifier) 
	{ 
		return false; 
	}

	bool InputEventReceiver::OnCharEvent(unsigned int c) 
	{ 
		return false; 
	}

}; // namespace chaos
