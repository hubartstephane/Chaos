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
		class WindowPixelCaptureResult
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
		bool IsApplicationForeground();

		/** returns true whether the keyboard is AZERTY */
		bool IsAzertyKeyboard();

		/** create a "Press to continue ..." pause console */
		void PressToContinue();

		/** create input/output console */
		bool AllocConsoleAndRedirectStdOutput();

		/** test whether there is an error, display the message into the logs */
		void DisplayErrorMessage(char const* title = nullptr);

		/** get the string from an error code */
		std::string GetErrorString(DWORD errCode);

		/** use the windows register to "open" a file */
		void ShowFile(FilePathParam const& path);

		/** returns the current user Document path */
		boost::filesystem::path GetUserDocumentPath();

		/** returns the current use local path */
		boost::filesystem::path GetUserLocalPath();

		/** returns the current user Desktop path */
		boost::filesystem::path GetUserDesktopPath();

		/** copies a string into the clipboard */
		bool CopyStringToClipboard(char const* str);

		/** copies a HBITMAP into clipboard */
		bool CopyBitmapToClipboard(HBITMAP hbitmap);
		/** copies a FIBITMAP into clipboard */
		bool CopyBitmapToClipboard(FIBITMAP* bitmap);

		/** capture a windows inside a HBITMAP */
		HBITMAP CaptureWindowToBitmap(HWND hWnd);
		/** capture a windows inside a HBITMAP */
		HBITMAP CaptureWindowToBitmap(HDC hDC);

		/** capture a windows inside the clipboard */
		bool CaptureWindowToClipboard(HWND hWnd);
		/** capture a windows inside the clipboard */
		bool CaptureWindowToClipboard(HDC hDC);

		/** capture a windows inside a buffer */
		WindowPixelCaptureResult CaptureBitmapToPixelBuffer(HBITMAP hBitmap, HDC hDC);
		/** capture a windows inside a buffer */
		WindowPixelCaptureResult CaptureWindowToPixelBuffer(HWND hWnd);
		/** capture a windows inside a buffer */
		WindowPixelCaptureResult CaptureWindowToPixelBuffer(HDC hDC);
		/** capture a windows inside an image */
		FIBITMAP* CaptureWindowToImage(HWND hWnd);
		/** capture a windows inside an image */
		FIBITMAP* CaptureWindowToImage(HDC hDC);

		/** enumerate the monitors (ordered left to right) */
		std::vector<MONITORINFOEX> EnumerateMonitors();

#endif

	}; // namespace WinTools

}; // namespace chaos