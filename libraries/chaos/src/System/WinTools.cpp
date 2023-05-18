#include "chaos/ChaosPCH.h"
#include <Chaos/Chaos.h>

namespace chaos
{
	namespace WinTools
	{
		static boost::filesystem::path GetKnownFolderPath(REFKNOWNFOLDERID folder_id)
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


		bool IsApplicationForeground()
		{
			std::uint32_t foreground_process;
			::GetWindowThreadProcessId(GetForegroundWindow(), (::DWORD*) & foreground_process);
			return (foreground_process == GetCurrentProcessId());
		}

		bool IsAzertyKeyboard()
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

		void PressToContinue()
		{
			system("pause");
		}

#if 0

		// XXX : study how redirect STDOUT, STDERR, STDIN into Console. For the moment i do understand nothing on that

		// https://stackoverflow.com/questions/311955/redirecting-cout-to-a-console-in-windows

		static void BindCrtHandlesToStdHandles(bool bindStdIn, bool bindStdOut, bool bindStdErr)
		{
			// Re-initialize the C runtime "FILE" handles with clean handles bound to "nul". We do this because it has been
			// observed that the file number of our standard handle file objects can be assigned internally to a value of -2
			// when not bound to a valid target, which represents some kind of unknown internal invalid state. In this state our
			// call to "_dup2" fails, as it specifically tests to ensure that the target file number isn't equal to this value
			// before allowing the operation to continue. We can resolve this issue by first "re-opening" the target files to
			// use the "nul" device, which will place them into a valid state, after which we can redirect them to our target
			// using the "_dup2" function.
			if (bindStdIn)
			{
				FILE* dummyFile;
				freopen_s(&dummyFile, "nul", "r", stdin);
			}
			if (bindStdOut)
			{
				FILE* dummyFile;
				freopen_s(&dummyFile, "nul", "w", stdout);
			}
			if (bindStdErr)
			{
				FILE* dummyFile;
				freopen_s(&dummyFile, "nul", "w", stderr);
			}

			// Redirect unbuffered stdin from the current standard input handle
			if (bindStdIn)
			{
				HANDLE stdHandle = GetStdHandle(STD_INPUT_HANDLE);
				if (stdHandle != INVALID_HANDLE_VALUE)
				{
					int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
					if (fileDescriptor != -1)
					{
						FILE* file = _fdopen(fileDescriptor, "r");
						if (file != NULL)
						{
							int dup2Result = _dup2(_fileno(file), _fileno(stdin));
							if (dup2Result == 0)
							{
								setvbuf(stdin, NULL, _IONBF, 0);
							}
						}
					}
				}
			}

			// Redirect unbuffered stdout to the current standard output handle
			if (bindStdOut)
			{
				HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
				if (stdHandle != INVALID_HANDLE_VALUE)
				{
					int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
					if (fileDescriptor != -1)
					{
						FILE* file = _fdopen(fileDescriptor, "w");
						if (file != NULL)
						{
							int dup2Result = _dup2(_fileno(file), _fileno(stdout));
							if (dup2Result == 0)
							{
								setvbuf(stdout, NULL, _IONBF, 0);
							}
						}
					}
				}
			}

			// Redirect unbuffered stderr to the current standard error handle
			if (bindStdErr)
			{
				HANDLE stdHandle = GetStdHandle(STD_ERROR_HANDLE);
				if (stdHandle != INVALID_HANDLE_VALUE)
				{
					int fileDescriptor = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
					if (fileDescriptor != -1)
					{
						FILE* file = _fdopen(fileDescriptor, "w");
						if (file != NULL)
						{
							int dup2Result = _dup2(_fileno(file), _fileno(stderr));
							if (dup2Result == 0)
							{
								setvbuf(stderr, NULL, _IONBF, 0);
							}
						}
					}
				}
			}

			// Clear the error state for each of the C++ standard stream objects. We need to do this, as attempts to access the
			// standard streams before they refer to a valid target will cause the iostream objects to enter an error state. In
			// versions of Visual Studio after 2005, this seems to always occur during startup regardless of whether anything
			// has been read from or written to the targets or not.
			if (bindStdIn)
			{
				std::wcin.clear();
				std::cin.clear();
			}
			if (bindStdOut)
			{
				std::wcout.clear();
				std::cout.clear();
			}
			if (bindStdErr)
			{
				std::wcerr.clear();
				std::cerr.clear();
			}
		}

#endif

		static void FlushHelpTextMessages()
		{
			// display messages that were pending
			size_t flushed_message_count = HelpText::FlushMessages([](char const* family, char const* message)
			{
				Log::BeginTransaction(LogType::Message);
				// starting a new family
				if (message == nullptr)
				{
					std::cout << "==========================================" << '\n';
					std::cout << "== " << family << '\n';
					std::cout << "==========================================" << '\n';
					std::cout << '\n';
				}
				// all messages of this family
				else
				{
					std::cout << "  " << message << '\n';
				}
				Log::EndTransaction();
			});
		}

		// XXX : WindowedApp(...) required, does not work with ConsoleApp(...)
		bool AllocConsoleAndRedirectStdOutput()
		{
			if (AllocConsole())
			{
				// XXX : i found on internet several versions that don't work correctly
				//       this one seems to :
				//         - works correctly with FILE * objects   stdout, stderr and stdin
				//         - works correctly with std:: objects    std::cin, std::cout and std::cin
				//       but
				//         - not working correctly with file descriptor 0, 1, 2

				FILE* my_stdout = stdout;
				FILE* my_stderr = stderr; // because stdin, stdout and stderr are under the hood
				FILE* my_stdin = stdin;  // function calls __acrt_iob_func(0, 1, 2)

				freopen_s(&my_stdout, "CON", "w", stdout);
				freopen_s(&my_stderr, "CON", "w", stderr);
				freopen_s(&my_stdin, "CON", "r", stdin);

				for (int i = 0; i < 3; ++i)
				{
					DWORD handles[] = { STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE };

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

				// XXX : seems important to clear previous error flags of theses STD STREAM (for call to std::cout << XXX    that could happens before the Console is opened)
				std::wcin.clear();
				std::cin.clear();
				std::wcout.clear();
				std::cout.clear();
				std::wcerr.clear();
				std::cerr.clear();

				// change buffer size
				COORD bufsize;
				bufsize.X = 1600;
				bufsize.Y = 2000;
				SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufsize);

				// XXX : some tests for console color. Not working yet

#if 0

			// https://en.wikipedia.org/wiki/ANSI_escape_code
			// https://stackoverflow.com/questions/2348000/colors-in-c-win32-console

				const char* green = "\033[0;32m";
				const char* white = "\033[0;37m";
				const char* red = "\033[0;31m";

				std::cout << "toto" << green << "titi" << white << "tutu" << red << "tyty" << '\n';
				std::cout << "toto" << green << "titi" << white << "tutu" << red << "tyty" << '\n';

#endif

				FlushHelpTextMessages();

				return true;
			}

			return false;
		}

		void DisplayErrorMessage(char const* title)
		{
			DWORD err_code = GetLastError();
			if (err_code != 0)
			{
				std::string err_msg = GetErrorString(err_code);

				// remove end of lines
				err_msg = StringTools::RemoveEndOfLines(std::move(err_msg));

				if (title == nullptr)
					Log::Error("Error : [%s]", err_msg.c_str());
				else
					Log::Error("Error in [%s] : [%s]", title, err_msg.c_str());
			}
		}

		std::string GetErrorString(DWORD errCode)
		{
			assert(errCode != 0);

			LPSTR buffer = nullptr;
			size_t size = FormatMessageA(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPSTR)&buffer, 0, NULL);

			std::string result(buffer, size);

			LocalFree(buffer); //Free the buffer.
			return result;
		}

		void ShowFile(FilePathParam const& path)
		{
			boost::filesystem::path const& resolved_path = path.GetResolvedPath();
			ShellExecuteA(nullptr, "open", resolved_path.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);     // XXX: require CoInitialize(nullptr);
		}

		boost::filesystem::path GetUserLocalPath()
		{
			return GetKnownFolderPath(FOLDERID_LocalAppData); // FOLDERID_RoamingAppData
		}

		boost::filesystem::path GetUserDocumentPath()
		{
			return GetKnownFolderPath(FOLDERID_Documents);
		}

		boost::filesystem::path GetUserDesktopPath()
		{
			return GetKnownFolderPath(FOLDERID_Desktop);
		}

		bool CopyStringToClipboard(char const* str)
		{
			assert(str != nullptr);

			size_t len = strlen(str);
			if (len == 0)
				return false;

			bool result = false;

			if (OpenClipboard(NULL))
			{
				EmptyClipboard();

				HLOCAL hMem = LocalAlloc(LHND, len + 1);
				if (hMem != NULL)
				{
					char* cptr = (char*)LocalLock(hMem);
					if (cptr != nullptr)
					{
						strcpy_s(cptr, len + 1, str);

						if (SetClipboardData(CF_TEXT, hMem))
							result = true;
						LocalUnlock(hMem);
					}
					LocalFree(hMem);
				}
				CloseClipboard();
			}
			return result;
		}


		bool CopyBitmapToClipboard(FIBITMAP* bitmap)
		{
#if 0


			HBITMAP hBitmap = ImageTools::ConvertToHBITMAP(bitmap);
			if (hBitmap == NULL)
				return false;
			bool result = CopyBitmapToClipboard(hBitmap);
			DeleteObject(hBitmap);
			return result;
#endif
			return false;
		}

		bool CopyBitmapToClipboard(HBITMAP hBitmap)
		{
			bool result = false;
			if (OpenClipboard(NULL))
			{
				EmptyClipboard();
				if (SetClipboardData(CF_BITMAP, hBitmap))
					result = true;
				CloseClipboard();
			}
			return result;
		}

		HBITMAP CaptureWindowToBitmap(HWND hWnd)
		{
			return CaptureWindowToBitmap(GetDC(hWnd));
		}

		HBITMAP CaptureWindowToBitmap(HDC hDC)
		{
			HBITMAP result = NULL;

			if (hDC != NULL)
			{
				HDC hDestDC = CreateCompatibleDC(hDC);
				if (hDestDC != NULL)
				{
					int width = 0;
					int height = 0;

					HWND hWnd = WindowFromDC(hDC);
					if (hWnd != NULL)
					{
						RECT r;
						if (GetClientRect(hWnd, &r))
						{
							width = r.right - r.left;
							height = r.bottom - r.top;
						}
					}
					else // for full screen
					{
						width = GetDeviceCaps(hDC, HORZRES);
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

		bool CaptureWindowToClipboard(HWND hWnd)
		{
			return CaptureWindowToClipboard(GetDC(hWnd));
		}

		bool CaptureWindowToClipboard(HDC hDC)
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

		WindowPixelCaptureResult CaptureBitmapToPixelBuffer(HBITMAP hBitmap, HDC hDC)
		{
			SIZE bitmap_size;
			if (::GetBitmapDimensionEx(hBitmap, &bitmap_size))
			{
				WindowPixelCaptureResult result;
				result.width = int(bitmap_size.cx);
				result.height = int(bitmap_size.cy);

				result.pixels = SharedBufferPolicy<char>::NewBuffer(result.width * result.height * 3);

				if (result.pixels != nullptr) // ... maybe an issue while allocation do not come from GlobalAlloc(...)
				{
					BITMAPINFOHEADER bmi = { 0 };
					bmi.biSize = sizeof(BITMAPINFOHEADER);
					bmi.biPlanes = 1;
					bmi.biBitCount = 24;
					bmi.biWidth = result.width;
					bmi.biHeight = result.height;
					bmi.biCompression = BI_RGB;
					bmi.biSizeImage = result.height * result.width;

					if (GetDIBits(hDC, hBitmap, 0, result.height, result.pixels.data, (BITMAPINFO*)&bmi, DIB_RGB_COLORS) != 0)
						return result;
				}
			}
			return WindowPixelCaptureResult();
		}

		WindowPixelCaptureResult CaptureWindowToPixelBuffer(HWND hWnd)
		{
			return CaptureWindowToPixelBuffer(GetDC(hWnd));
		}

		WindowPixelCaptureResult CaptureWindowToPixelBuffer(HDC hDC)
		{
			WindowPixelCaptureResult result;

			HBITMAP hBitmap = CaptureWindowToBitmap(hDC);
			result = CaptureBitmapToPixelBuffer(hBitmap, hDC); // xxx: maybe the DC should come from hDestDC ...
			DeleteObject(hBitmap);

			return result;
		}

		FIBITMAP* CaptureWindowToImage(HWND hWnd)
		{
			return CaptureWindowToImage(GetDC(hWnd));
		}

		FIBITMAP* CaptureWindowToImage(HDC hDC)
		{
			FIBITMAP* result = nullptr;

			HBITMAP hBitmap = CaptureWindowToBitmap(hDC);
			if (hBitmap != NULL)
			{
				SIZE bitmap_size;
				if (::GetBitmapDimensionEx(hBitmap, &bitmap_size))
				{
					int width = int(bitmap_size.cx);
					int height = int(bitmap_size.cy);

					result = FreeImage_Allocate(width, height, 24);
					if (result != nullptr)
					{
						bool success = false;

						BYTE* bytes = FreeImage_GetBits(result);
						if (bytes != nullptr)
						{
							BITMAPINFOHEADER bmi = { 0 };
							bmi.biSize = sizeof(BITMAPINFOHEADER);
							bmi.biPlanes = 1;
							bmi.biBitCount = 24;
							bmi.biWidth = width;
							bmi.biHeight = height;
							bmi.biCompression = BI_RGB;
							bmi.biSizeImage = height * width;

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
					BYTE* bytes = FreeImage_GetBits(result);
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
			std::vector<MONITORINFOEX>* result = (std::vector<MONITORINFOEX>*)(dwData);

			MONITORINFOEX monitor;
			memset(&monitor, 0, sizeof(monitor));
			monitor.cbSize = sizeof(monitor);

			if (GetMonitorInfo(hMonitor, &monitor))
				result->push_back(monitor);
			return TRUE;
		}

		std::vector<MONITORINFOEX> EnumerateMonitors()
		{
			std::vector<MONITORINFOEX> result;

			EnumDisplayMonitors(NULL, NULL, MyEnumMonitorFunc, (LPARAM)&result); // retrieve number of monitor & monitor size

			std::sort(result.begin(), result.end(), [](MONITORINFOEX const& src1, MONITORINFOEX const& src2)
			{
				if (src1.rcMonitor.left < src2.rcMonitor.left)
					return true;
				return false;
			});

			return result;
		}





	}; // namespace WinTools

}; // namespace chaos
