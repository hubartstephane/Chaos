#include "chaos/Chaos.h"

namespace chaos
{

	SimpleWin32ClassBase::SimpleWin32ClassBase(char const * in_window_classname) : window_classname(in_window_classname)
	{
		WNDCLASSA wc = { };

		bool native_class = false;

		wc.lpfnWndProc   = native_class? DefWindowProc : SimpleWin32Window::WindowProc;
		wc.hInstance     = NULL;
		wc.lpszClassName = window_classname;  

		if (!native_class)
			RegisterClassA(&wc);
	}

	SimpleWin32Window * SimpleWin32ClassBase::DoCreateWindowHinstance(HWND hParent, char const * window_title, SimpleWin32Window * result, SimpleWin32CreateParam const & create_params)
	{
		if (result != NULL)
		{
			HWND hWnd = CreateWindowExA(
				create_params.dwExStyle, // Optional window styles.
				window_classname,        // Window class
				window_title,            // Window text
				create_params.dwStyle,   // Window style        
				create_params.x,         // Size and position        
				create_params.y,         // Size and position        
				create_params.nWidth,    // Size and position        
				create_params.nHeight,   // Size and position        
				hParent,                 // Parent window    
				NULL,                    // Menu
				NULL,                    // Instance handle
				(LPVOID)result           // Additional application data
			);

			if (hWnd == NULL)
			{
				delete result;
				return NULL;
			}
			result->PostCreateWindow();
		}       
		return result;
	}

}; // namespace chaos
