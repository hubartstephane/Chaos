namespace chaos
{
	/**
	* WinTools is an utility class with a number of tool function
	*/
	namespace WinTools
	{

#ifdef CHAOS_FORWARD_DECLARATION

		class WindowPixelCaptureResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION


		/** a structure that contains the result of a capture of a window */
		class CHAOS_API WindowPixelCaptureResult
		{
		public:

			/** the buffer that contains pixels RGB or RGBA */
			Buffer<char> pixels;
			/** the size of the image */
			int width = 0;
			/** the size of the image */
			int height = 0;
		};

		/** returns true whether the application has focus */
		CHAOS_API bool IsApplicationForeground();

		/** returns true whether the keyboard is AZERTY */
		CHAOS_API bool IsAzertyKeyboard();

		/** create a "Press to continue ..." pause console */
		CHAOS_API void PressToContinue();

		/** create input/output console */
		CHAOS_API bool AllocConsoleAndRedirectStdOutput();

		/** test whether there is an error, display the message into the logs */
		CHAOS_API void DisplayErrorMessage(char const* title = nullptr);

		/** get the string from an error code */
		CHAOS_API std::string GetErrorString(DWORD errCode);

		/** use the windows register to "open" a file */
		CHAOS_API void ShowFile(FilePathParam const& path);

		/** returns the current user Document path */
		CHAOS_API boost::filesystem::path GetUserDocumentPath();

		/** returns the current use local path */
		CHAOS_API boost::filesystem::path GetUserLocalPath();

		/** returns the current user Desktop path */
		CHAOS_API boost::filesystem::path GetUserDesktopPath();

		/** copies a string into the clipboard */
		CHAOS_API bool CopyStringToClipboard(char const* str);

		/** copies a HBITMAP into clipboard */
		CHAOS_API bool CopyBitmapToClipboard(HBITMAP hbitmap);
		/** copies a FIBITMAP into clipboard */
		CHAOS_API bool CopyBitmapToClipboard(FIBITMAP* bitmap);

		/** capture a windows inside a HBITMAP */
		CHAOS_API HBITMAP CaptureWindowToBitmap(HWND hWnd);
		/** capture a windows inside a HBITMAP */
		CHAOS_API HBITMAP CaptureWindowToBitmap(HDC hDC);

		/** capture a windows inside the clipboard */
		CHAOS_API bool CaptureWindowToClipboard(HWND hWnd);
		/** capture a windows inside the clipboard */
		CHAOS_API bool CaptureWindowToClipboard(HDC hDC);

		/** capture a windows inside a buffer */
		CHAOS_API WindowPixelCaptureResult CaptureBitmapToPixelBuffer(HBITMAP hBitmap, HDC hDC);
		/** capture a windows inside a buffer */
		CHAOS_API WindowPixelCaptureResult CaptureWindowToPixelBuffer(HWND hWnd);
		/** capture a windows inside a buffer */
		CHAOS_API WindowPixelCaptureResult CaptureWindowToPixelBuffer(HDC hDC);
		/** capture a windows inside an image */
		CHAOS_API FIBITMAP* CaptureWindowToImage(HWND hWnd);
		/** capture a windows inside an image */
		CHAOS_API FIBITMAP* CaptureWindowToImage(HDC hDC);

		/** enumerate the monitors (ordered left to right) */
		CHAOS_API std::vector<MONITORINFOEX> EnumerateMonitors();

#endif

	}; // namespace WinTools

}; // namespace chaos