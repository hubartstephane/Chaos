namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SimpleWin32Window;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* SimpleWin32Window : a class handle some on-the-shelve window
	*/

	class CHAOS_API SimpleWin32Window
	{
	public:

		friend class SimpleWin32ClassBase;

		/** constructor */
		SimpleWin32Window();

		/** Enable window to accept drag and drop */
		void DragAcceptFiles(BOOL bAccept);
		/** Show hide the window */
		void ShowWindow(int nCmdShow);

		/** an utility function to handle all window (if callback is given use PEEK, use blocking GET elsewhere) */
		static void SimpleMessageLoop(LightweightFunction<void()> func = {});

	protected:

		/** called whenever Windows size changed */
		virtual LRESULT OnWindowSize(int width, int height);
		/** called whenever a file is being dropped */
		virtual BOOL OnDragFile(char const* Filename, POINT const& pt);
		/** handle a dropped file */
		virtual LRESULT OnDropFile(HDROP hDrop);
		/** some mouse events MOUSE_MOVE */
		virtual LRESULT OnMouseMove(int x, int y, int buttonStates);
		/** some mouse events MOUSE_LEFT_DOWN */
		virtual LRESULT OnLButtonDown(int x, int y, int buttonStates);
		/** some mouse events MOUSE_LEFT_UP */
		virtual LRESULT OnLButtonUp(int x, int y, int buttonStates);
		/** some mouse events MOUSE_RIGHT_DOWN */
		virtual LRESULT OnRButtonDown(int x, int y, int buttonStates);
		/** some mouse events MOUSE_RIGHT_UP */
		virtual LRESULT OnRButtonUp(int x, int y, int buttonStates);
		/** some mouse events MOUSE_MIDDLE_DOWN */
		virtual LRESULT OnMButtonDown(int x, int y, int buttonStates);
		/** some mouse events MOUSE_MIDDLE_UP */
		virtual LRESULT OnMButtonUp(int x, int y, int buttonStates);
		/** some mouse events MOUSE_WHEEL */
		virtual LRESULT OnMouseWheel(int x, int y, int wheel_value, int buttonStates);
		/** some key event KEYDOWN */
		virtual LRESULT OnKeyDown(int keycode, int scancode);
		/** some key event KEYUP */
		virtual LRESULT OnKeyUp(int keycode, int scancode);
		/** called by WM_DESTROY */
		virtual LRESULT OnDestroy();

		/** the rendering method */
		virtual LRESULT OnPaint();
		/** called whenever the background is to be erased */
		virtual LRESULT OnEraseBackground(HDC hDC);
		/** the window creation callback */
		virtual LRESULT OnCreate(CREATESTRUCTA* create_param);
		/** the method that is beeing called after the window creation */
		virtual void PostCreateWindow();

		/** returns the HWND handler */
		inline HWND GetHwnd() const { return hWnd; }

	protected:

		/** the default message dispatch function */
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // the default static Window Loop method
																								/** the specific message handler */
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& message_handled);
		/** called whenever the window is being created */
		static LRESULT BindWindow(HWND hWnd, CREATESTRUCTA* create_param);

	protected:

		/** the handler of the window */
		HWND hWnd = NULL;
	};

#endif

}; // namespace chaos