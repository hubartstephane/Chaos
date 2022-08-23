#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	SimpleWin32Window::SimpleWin32Window(): hWnd(NULL)
	{

	}

	void SimpleWin32Window::SimpleMessageLoop(std::function<void()> func)
	{
		MSG msg = { };
		while (msg.message != WM_QUIT)
		{
			// with callback => PEEK
			if (func)
			{
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				func();
			}
			// without callback => GET (blocking)
			else
			{
				while (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}

	LRESULT SimpleWin32Window::OnWindowSize(int width, int height)
	{
		return 0;
	}

	BOOL SimpleWin32Window::OnDragFile( char const * Filename, POINT const & pt)
	{
		return TRUE;
	}

	LRESULT SimpleWin32Window::OnDropFile(HDROP hDrop)
	{
		char Filename[MAX_PATH];

		UINT FileCount = DragQueryFileA(hDrop, 0xFFFFFFFF, Filename, sizeof(Filename) / sizeof(Filename[0]));
		if (FileCount != 0)
		{
			POINT pt;
			DragQueryPoint(hDrop, &pt);

			for (UINT i = 0 ; i < FileCount ; ++i)
			{
				if (DragQueryFileA(hDrop, i, Filename, sizeof(Filename) / sizeof(Filename[0])) != 0)
				{
					if (!OnDragFile(Filename, pt))
						break;       
				}
			}
		}

		DragFinish(hDrop);
		return 0;
	}

	LRESULT SimpleWin32Window::BindWindow(HWND hWnd, CREATESTRUCTA * create_param)
	{
		SimpleWin32Window * myself = (SimpleWin32Window *)create_param->lpCreateParams;
		if (myself != NULL)
		{
			myself->hWnd = hWnd;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)myself); // set the window creation data
			return 0;
		}
		return -1;
	}

	LRESULT SimpleWin32Window::OnEraseBackground(HDC hDC)
	{
		return 0;
	}

	LRESULT SimpleWin32Window::OnCreate(CREATESTRUCTA * create_param)
	{
		return 0;
	}

	LRESULT SimpleWin32Window::OnMouseMove(int x, int y, int buttonStates) // (0, 0) = TOPLEFT
	{
		return 0;
	}

	LRESULT SimpleWin32Window::OnLButtonDown(int x, int y, int buttonStates) 
	{
		return 0;
	}

	LRESULT SimpleWin32Window::OnLButtonUp(int x, int y, int buttonStates) 
	{
		return 0;
	}

	LRESULT SimpleWin32Window::OnRButtonDown(int x, int y, int buttonStates) 
	{
		return 0;
	}

	LRESULT SimpleWin32Window::OnRButtonUp(int x, int y, int buttonStates) 
	{
		return 0;
	}

	LRESULT SimpleWin32Window::OnMButtonDown(int x, int y, int buttonStates) 
	{
		return 0;
	}

	LRESULT SimpleWin32Window::OnMButtonUp(int x, int y, int buttonStates) 
	{
		return 0;
	}

	LRESULT SimpleWin32Window::OnMouseWheel(int x, int y, int wheel_value, int buttonStates)
	{
		return 0;  
	}

	LRESULT SimpleWin32Window::OnPaint()
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_DESKTOP + 1));
		EndPaint(hWnd, &ps);
		return 0;
	}

	LRESULT SimpleWin32Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & message_handled)
	{
		message_handled = TRUE;

		switch (uMsg)
		{
		case WM_PAINT:
			return OnPaint();

		case WM_ERASEBKGND:
			return OnEraseBackground((HDC)wParam);

#define CHAOS_CATCH_WND_MSG(MSG, FUNC)\
    case MSG:\
      return FUNC(int(GET_X_LPARAM(lParam)), int(GET_Y_LPARAM(lParam)), int(GET_KEYSTATE_WPARAM(wParam)));
			CHAOS_CATCH_WND_MSG(WM_MOUSEMOVE,   OnMouseMove);
			CHAOS_CATCH_WND_MSG(WM_LBUTTONDOWN, OnLButtonDown);
			CHAOS_CATCH_WND_MSG(WM_LBUTTONUP,   OnLButtonUp);
			CHAOS_CATCH_WND_MSG(WM_RBUTTONDOWN, OnRButtonDown);
			CHAOS_CATCH_WND_MSG(WM_RBUTTONUP,   OnRButtonUp);
			CHAOS_CATCH_WND_MSG(WM_MBUTTONDOWN, OnMButtonDown);
			CHAOS_CATCH_WND_MSG(WM_MBUTTONUP,   OnMButtonUp);
#undef CHAOS_CATCH_WND_MSG

		case WM_MOUSEWHEEL:
			return OnMouseWheel(int(GET_X_LPARAM(lParam)), int(GET_Y_LPARAM(lParam)), int(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA, int(GET_KEYSTATE_WPARAM(wParam))); 

		case WM_SIZE:
			return OnWindowSize(int(LOWORD(lParam)), int(HIWORD(lParam)));

		case WM_DROPFILES:
			return OnDropFile((HDROP)wParam);

		case WM_CREATE:
			return OnCreate((CREATESTRUCTA*)lParam);

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			message_handled = FALSE;
		}
		return 0;
	}

	LRESULT CALLBACK SimpleWin32Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CREATE)
			BindWindow(hWnd, (CREATESTRUCTA *)lParam);

		SimpleWin32Window * myself = (SimpleWin32Window *)GetWindowLongPtr(hWnd, GWLP_USERDATA); // data is not available until WM_CREATE has been called
		if (myself != NULL)
		{
			BOOL message_handled = FALSE; 
			LRESULT result = myself->HandleMessage(uMsg, wParam, lParam, message_handled);
			if (message_handled)
				return result;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void SimpleWin32Window::DragAcceptFiles(BOOL bAccept)
	{
		::DragAcceptFiles(hWnd, bAccept);
	}

	void SimpleWin32Window::ShowWindow(int nCmdShow)
	{
		::ShowWindow(hWnd, nCmdShow);
	}

	void SimpleWin32Window::PostCreateWindow()
	{
		DragAcceptFiles(TRUE);
		ShowWindow(SW_SHOWNORMAL);
	}

}; // namespace chaos
