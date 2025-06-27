#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	WindowFrameSizeInfo GetWindowFrameSizeInfo(GLFWwindow* window)
	{
		WindowFrameSizeInfo result;
		if (window != nullptr)
		{
			glfwGetWindowFrameSize(window, &result.left, &result.top, &result.right, &result.bottom);

			// XXX: the left and right values are surestimated
			//      without the following correction we would have the following bug
			//      1/double click on the title bar so that the window fills the right monitors (but still with a title bar)
			//      2/press F10 so that the window really becomes fullscreen (no more decorators)
			//      3/observe that the window is now on the left monitor
			//        -> because the effective top-left corner (that is to say not the client position) is a bit on the left monitor making
			//           left monitor preferred
			result.left = 0;
			result.right = 0;
		}
		return result;
	}

}; // namespace chaos
