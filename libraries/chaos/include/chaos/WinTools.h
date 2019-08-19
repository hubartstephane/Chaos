#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/buffer.h>
#include <chaos/FilePath.h>


namespace chaos
{

	/**
	* WinTools is an utility class with a number of tool function
	*/
	class WinTools
	{
	public:

		/** returns true whether the application has focus */
		static bool IsApplicationForeground();

		/** returns true whether the keyboard is AZERTY */
		static bool IsAzertyKeyboard();

		/** create a "Press to continue ..." pause console */
		static void PressToContinue();

		/** create input/output console */
		static bool AllocConsoleAndRedirectStdOutput();

		/** test whether there is an error, display the message into the logs */
		static void DisplayErrorMessage(char const * title = nullptr);

		/** get the string from an error code */
		static std::string WinTools::GetErrorString(DWORD errCode);

		/** use the windows register to "open" a file */
		static void ShowFile(FilePathParam const & path);

		/** returns the current user Document path */
		static boost::filesystem::path GetUserDocumentPath();

		/** returns the current use local path */
		static boost::filesystem::path GetUserLocalPath();

		/** returns the current user Desktop path */
		static boost::filesystem::path GetUserDesktopPath();

		/** copies a string into the clipboard */
		static bool CopyStringToClipboard(char const * str);

		/** copies a HBITMAP into clipboard */
		static bool CopyBitmapToClipboard(HBITMAP hbitmap);

		/** capture a windows inside a HBITMAP */
		static HBITMAP CaptureWindowToBitmap(HWND hWnd);
		/** capture a windows inside a HBITMAP */
		static HBITMAP CaptureWindowToBitmap(HDC hDC);

		/** capture a windows inside the clipboard */
		static bool CaptureWindowToClipboard(HWND hWnd);
		/** capture a windows inside the clipboard */
		static bool CaptureWindowToClipboard(HDC hDC);


		/** a structure that contains the result of a capture of a window */
		class WindowPixelCaptureResult
		{
		public:

			/** the buffer that contains pixels RGB or RGBA */
			Buffer<char> pixels;
			/** the size of the image */
			int width;
			/** the size of the image */
			int height;
		};

		/** capture a windows inside a buffer */
		static WindowPixelCaptureResult CaptureBitmapToPixelBuffer(HBITMAP hBitmap, HDC hDC);
		/** capture a windows inside a buffer */
		static WindowPixelCaptureResult CaptureWindowToPixelBuffer(HWND hWnd);
		/** capture a windows inside a buffer */
		static WindowPixelCaptureResult CaptureWindowToPixelBuffer(HDC hDC);
		/** capture a windows inside an image */
		static FIBITMAP * CaptureWindowToImage(HWND hWnd);
		/** capture a windows inside an image */
		static FIBITMAP * CaptureWindowToImage(HDC hDC);

		/** enumerate the monitors (ordered left to right) */
		static std::vector<MONITORINFOEX> EnumerateMonitors();

	private:

		static boost::filesystem::path GetKnownFolderPath(REFKNOWNFOLDERID folder_id);
	};

}; // namespace chaos

