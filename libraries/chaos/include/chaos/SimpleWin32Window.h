#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	/**
	* SimpleWin32Window : a class handle some on-the-shelve window
	*/

	class SimpleWin32Window
	{
	public:

		friend class SimpleWin32ClassBase;

		/** constructor */
		SimpleWin32Window();

		/** Enable window to accept drag and drop */
		void DragAcceptFiles(BOOL bAccept);
		/** Show hide the window */
		void ShowWindow(int nCmdShow);

		/** an utility function to handle all window : live loop with a special action */
		template<typename T>
		static void SimpleMessageLoop(T special_operation) // example => T = boost::log::BOOST_LOG_VERSION_NAMESPACE::nop()
		{
			MSG msg = { };
			while (msg.message != WM_QUIT)
			{
				while (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE) > 0) 
				{
					TranslateMessage (&msg);
					DispatchMessage (&msg);
				}
				special_operation();
			}
		}

		/** an utility function to handle all window */
		static void SimpleMessageLoop();

	protected:

		/** called whenever Windows size changed */
		virtual LRESULT OnWindowSize(int width, int height);
		/** called whenever a file is being dropped */
		virtual BOOL OnDragFile(char const * Filename, POINT const & pt);
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
		/** the rendering method */
		virtual LRESULT OnPaint();
		/** called whenever the background is to be erased */
		virtual LRESULT OnEraseBackground(HDC hDC);
		/** the window creation callback */
		virtual LRESULT OnCreate(CREATESTRUCTA * create_param);
		/** the method that is beeing called after the window creation */
		virtual void PostCreateWindow();

		/** returns the HWND handler */
		inline HWND GetHwnd() const { return hWnd; }

	protected:

		/** the default message dispatch function */
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // the default static Window Loop method
																								/** the specific message handler */
		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & message_handled);
		/** called whenever the window is being created */
		static LRESULT BindWindow(HWND hWnd, CREATESTRUCTA * create_param);

	protected:

		/** the handler of the window */
		HWND hWnd;
	};


}; // namespace chaos
