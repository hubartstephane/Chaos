namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	class WindowFrameSizeInfo;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* WindowFrameSizeInfo: describes the decoration surface surrounding a window
	*/

	class CHAOS_API WindowFrameSizeInfo
	{
	public:

		int left = 0;
		int top = 0;
		int right = 0;
		int bottom = 0;
	};

	CHAOS_API WindowFrameSizeInfo GetWindowFrameSizeInfo(GLFWwindow* window);

#endif

}; // namespace chaos