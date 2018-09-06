#include <chaos/WinTools.h>
#include <chaos/AllocatorTools.h>
#include <chaos/LogTools.h>
#include <chaos/StringTools.h>

namespace chaos
{
	bool WinTools::IsAzertyKeyboard()
	{

		// Azerty layouts are not for all french keyboards
		// See layouts http://msdn.microsoft.com/en-us/goglobal/bb964651
		// See french langs IDs http://msdn.microsoft.com/en-us/library/windows/desktop/dd318693(v=vs.85).aspx

		HKL hKeyboardLayout = GetKeyboardLayout(NULL);
		WORD wLangId = LOWORD(hKeyboardLayout);
		switch (wLangId)
		{
		case MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_MONACO):
		case MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH_BELGIAN):
		case MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH): 
			return true;			
		}
		return false;
	}

	void WinTools::PressToContinue()
	{
		system("pause");
	}

	// XXX : WindowedApp(...) required, does not work with ConsoleApp(...)
	bool WinTools::AllocConsoleAndRedirectStdOutput()
	{
		if (AllocConsole())
		{
#if 1

			// XXX : i found on internet several versions that don't work correctly
			//       this one seems to :
			//         - works correctly with FILE * objects   stdout, stderr and stdin
			//         - works correctly with std:: objects    std::cin, std::cout and std::cin
			//       but
			//         - not working correctly with file descriptor 0, 1, 2

			FILE * my_stdout = stdout;
			FILE * my_stderr = stderr; // because stdin, stdout and stderr are under the hood
			FILE * my_stdin  = stdin;  // function calls __acrt_iob_func(0, 1, 2)

			freopen_s(&my_stdout, "CON", "w", stdout);
			freopen_s(&my_stderr, "CON", "w", stderr);
			freopen_s(&my_stdin,  "CON", "r", stdin);

			for (int i = 0 ; i < 3 ; ++i)
			{
				DWORD handles[] = {STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE};

				HANDLE hConsole = GetStdHandle(handles[i]);
				if (hConsole != INVALID_HANDLE_VALUE)
				{
					int iFile = _open_osfhandle((intptr_t)hConsole, _O_TEXT);
					if (iFile != -1)
					{
						if (iFile != 0 && iFile != 1 && iFile != 2) //  the output redirection should now work with file descriptor
						{
							_close(i);
							_dup(iFile);
						}
					}
				}
			}

			COORD bufsize;
			bufsize.X = 1600;
			bufsize.Y = 2000;
			if (!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufsize))
				DisplayErrorMessage("AllocConsoleAndRedirectStdOutput : SetConsoleScreenBufferSize(...)");

			return true;

#else
			// XXX : this version below works with   stdout  but not with descriptor methods

			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			if (hConsole != INVALID_HANDLE_VALUE)
			{
				int iFile = _open_osfhandle((intptr_t)hConsole, _O_TEXT);
				if (iFile != -1)
				{
					FILE * f = _fdopen(iFile, "w");
					if (f != nullptr)
					{
						// change the flags : _IONBF => no buffer
						// setvbuf(f, NULL, _IONBF, 0); 

						// change the flags : _IOLBF => buffer until eol
						static char buffer[1024];
						setvbuf(f, buffer, _IOLBF, sizeof(buffer)); // change the flags : _IOLBF => buffer until eol, _IONBF => no buffer

						std::ofstream * stream = new std::ofstream(f);
						auto backup = std::cout.rdbuf();
						std::cout.rdbuf(stream->rdbuf());

						COORD bufsize;
						bufsize.X = 1600;
						bufsize.Y = 2000;
						if (!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufsize))
							DisplayErrorMessage("AllocConsoleAndRedirectStdOutput : SetConsoleScreenBufferSize(...)");

#if 0
						SMALL_RECT rect;
						rect.Left = 0;
						rect.Right = 1200;
						rect.Top = 0;
						rect.Bottom = 800;
						if (!SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect))
							DisplayErrorMessage("AllocConsoleAndRedirectStdOutput : SetConsoleWindowInfo(...)");
#endif

						return true;
					}
				}
			}
#endif
		}

		DisplayErrorMessage("AllocConsoleAndRedirectStdOutput : AllocConsole(...)");

		return false;
	}

	void WinTools::DisplayErrorMessage(char const * title)
	{
		DWORD err_code = GetLastError();
		if (err_code != 0)
		{
			std::string err_msg = GetErrorString(err_code);

			// remove end of lines
			err_msg = StringTools::RemoveEndOfLines(std::move(err_msg));

			if (title == nullptr)
				LogTools::Error("Error : [%s]", err_msg.c_str());
			else
				LogTools::Error("Error in [%s] : [%s]", title, err_msg.c_str());
		}
	}

	std::string WinTools::GetErrorString(DWORD errCode)
	{
		assert(errCode != 0);

		LPSTR buffer = nullptr;
		size_t size  = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			errCode, 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPSTR)&buffer, 0, NULL);

		std::string result(buffer, size);    

		LocalFree(buffer); //Free the buffer.
		return result;
	}

	void WinTools::ShowFile(FilePathParam const & path) 
	{
		boost::filesystem::path const & resolved_path = path.GetResolvedPath();
		ShellExecuteA(nullptr, "open", resolved_path.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);     // XXX: require CoInitialize(nullptr);
	}

	boost::filesystem::path WinTools::GetUserLocalPath()
	{
		return GetKnownFolderPath(FOLDERID_LocalAppData); // FOLDERID_RoamingAppData
	}

	boost::filesystem::path WinTools::GetUserDocumentPath()
	{
		return GetKnownFolderPath(FOLDERID_Documents);
	}

	boost::filesystem::path WinTools::GetUserDesktopPath()
	{
		return GetKnownFolderPath(FOLDERID_Desktop);
	}

	boost::filesystem::path WinTools::GetKnownFolderPath(REFKNOWNFOLDERID folder_id)
	{
		boost::filesystem::path result;

		PWSTR p;
		if (SHGetKnownFolderPath(folder_id, 0, NULL, &p) == S_OK)
		{
			result = p;
			CoTaskMemFree(p);
		}
		return result;
	}

	bool WinTools::CopyBitmapToClipboard(HBITMAP hBitmap)
	{
		HANDLE handle = NULL;
		OpenClipboard(NULL);
		EmptyClipboard();
		handle = SetClipboardData(CF_BITMAP, hBitmap);
		CloseClipboard();
		return (handle != NULL);
	}

	HBITMAP WinTools::CaptureWindowToBitmap(HWND hWnd)
	{
		return CaptureWindowToBitmap(GetDC(hWnd)); 
	}

	HBITMAP WinTools::CaptureWindowToBitmap(HDC hDC)
	{
		HBITMAP result = NULL;

		if (hDC != NULL)
		{
			HDC hDestDC = CreateCompatibleDC(hDC);
			if (hDestDC != NULL)
			{
				int width  = 0;
				int height = 0;

				HWND hWnd = WindowFromDC(hDC);
				if (hWnd != NULL)
				{
					RECT r;
					if (GetClientRect(hWnd, &r))
					{
						width  = r.right  - r.left;
						height = r.bottom - r.top;
					}
				}
				else // for full screen
				{
					width  = GetDeviceCaps(hDC, HORZRES);
					height = GetDeviceCaps(hDC, VERTRES);
				}

				HBITMAP hBitmap = CreateCompatibleBitmap(hDC, width, height);
				if (hBitmap != NULL)
				{
					SelectObject(hDestDC, hBitmap);
					if (BitBlt(hDestDC, 0, 0, width, height, hDC, 0, 0, SRCCOPY))
					{
						result = hBitmap;
						SetBitmapDimensionEx(hBitmap, width, height, nullptr); // because ::GetBitmapDimensionEx(...) fails elsewhere
					}
					else
						DeleteObject(hBitmap);
				}
			}
			DeleteDC(hDestDC);
		}
		return result;
	}

	bool WinTools::CaptureWindowToClipboard(HWND hWnd)
	{
		return CaptureWindowToClipboard(GetDC(hWnd));
	}

	bool WinTools::CaptureWindowToClipboard(HDC hDC)
	{
		bool result = false;

		HBITMAP hBitmap = CaptureWindowToBitmap(hDC);
		if (hBitmap != NULL)
		{
			result = CopyBitmapToClipboard(hBitmap);
			DeleteObject(hBitmap);
		}
		return result;
	}

	WinTools::WindowPixelCaptureResult WinTools::CaptureBitmapToPixelBuffer(HBITMAP hBitmap, HDC hDC)
	{   
		SIZE bitmap_size;
		if (::GetBitmapDimensionEx(hBitmap, &bitmap_size))
		{    
			WindowPixelCaptureResult result;
			result.width  = (int)bitmap_size.cx;
			result.height = (int)bitmap_size.cy;

			result.pixels = SharedBufferPolicy<char>::NewBuffer(result.width * result.height * 3);

			if (result.pixels != nullptr) // ... maybe an issue while allocation do not come from GlobalAlloc(...)
			{
				BITMAPINFOHEADER bmi = {0};
				bmi.biSize        = sizeof(BITMAPINFOHEADER);
				bmi.biPlanes      = 1;
				bmi.biBitCount    = 24;
				bmi.biWidth       = result.width;
				bmi.biHeight      = result.height;
				bmi.biCompression = BI_RGB;
				bmi.biSizeImage   = result.height * result.width;

				if (GetDIBits(hDC, hBitmap, 0, result.height, result.pixels.data, (BITMAPINFO*)&bmi, DIB_RGB_COLORS) != 0)
					return result;
			}
		}
		return WindowPixelCaptureResult();
	}

	WinTools::WindowPixelCaptureResult WinTools::CaptureWindowToPixelBuffer(HWND hWnd)
	{
		return CaptureWindowToPixelBuffer(GetDC(hWnd));
	}

	WinTools::WindowPixelCaptureResult WinTools::CaptureWindowToPixelBuffer(HDC hDC)
	{
		WindowPixelCaptureResult result;

		HBITMAP hBitmap = CaptureWindowToBitmap(hDC);
		result = CaptureBitmapToPixelBuffer(hBitmap, hDC); // xxx: maybe the DC should come from hDestDC ...
		DeleteObject(hBitmap);

		return result;
	}

	FIBITMAP * WinTools::CaptureWindowToImage(HWND hWnd)
	{
		return CaptureWindowToImage(GetDC(hWnd));
	}

	FIBITMAP * WinTools::CaptureWindowToImage(HDC hDC)
	{
		FIBITMAP * result = nullptr;

		HBITMAP hBitmap = CaptureWindowToBitmap(hDC);
		if (hBitmap != NULL)
		{
			SIZE bitmap_size;
			if (::GetBitmapDimensionEx(hBitmap, &bitmap_size))
			{    
				int width  = (int)bitmap_size.cx;
				int height = (int)bitmap_size.cy;

				result = FreeImage_Allocate(width, height, 24);
				if (result != nullptr)
				{
					bool success = false;

					BYTE * bytes = FreeImage_GetBits(result);
					if (bytes != nullptr)
					{
						BITMAPINFOHEADER bmi = {0};
						bmi.biSize        = sizeof(BITMAPINFOHEADER);
						bmi.biPlanes      = 1;
						bmi.biBitCount    = 24;
						bmi.biWidth       = width;
						bmi.biHeight      = height;
						bmi.biCompression = BI_RGB;
						bmi.biSizeImage   = height * width;

						success = (GetDIBits(hDC, hBitmap, 0, height, bytes, (BITMAPINFO*)&bmi, DIB_RGB_COLORS) > 0);
					}

					if (!success)
					{
						FreeImage_Unload(result);
						result = nullptr;
					}
				}
			}

			DeleteObject(hBitmap);
		}

#if 0
		// naive implementation with double allocation. no interest
		WindowPixelCaptureResult capture = CaptureWindowToPixelBuffer(hDC);
		if (capture.pixels)
		{
			result = FreeImage_Allocate(capture.width, capture.height, 24);
			if (result != nullptr)
			{
				BYTE * bytes = FreeImage_GetBits(result);
				if (bytes != nullptr)
					memcpy(bytes, capture.pixels.data, capture.width * capture.height * 3);
			}
			capture.pixels.Release(true);
		}
#endif

		return result;
	}


	static BOOL CALLBACK MyEnumMonitorFunc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
	{
		std::vector<MONITORINFOEX> * result = (std::vector<MONITORINFOEX>*)(dwData);

		MONITORINFOEX monitor;
		memset(&monitor, 0, sizeof(monitor));
		monitor.cbSize = sizeof(monitor);

		if (GetMonitorInfo(hMonitor, &monitor))
			result->push_back(monitor);
		return TRUE;
	}

	std::vector<MONITORINFOEX> WinTools::EnumerateMonitors()
	{
		std::vector<MONITORINFOEX> result;

		EnumDisplayMonitors(NULL, NULL, MyEnumMonitorFunc, (LPARAM)&result); // retrieve number of monitor & monitor size

		std::sort(result.begin(), result.end(), [](MONITORINFOEX const & src1, MONITORINFOEX const & src2)
		{
			if (src1.rcMonitor.left < src2.rcMonitor.left)
				return true;
			return false;
		});

		return result;
	}
}; // namespace chaos
