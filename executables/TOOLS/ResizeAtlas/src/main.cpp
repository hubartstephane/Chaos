#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>
#include <chaos/ImageTools.h>

class MyWindow : public chaos::SimpleWin32Window
{
public:

	void WorkWithImage(char const* filename, FIBITMAP* bitmap)
	{

		chaos::WinTools::CopyBitmapToClipboard(bitmap);
		return;

		std::vector<FIBITMAP*> split_images;

		int ImageNumberW = 2;
		int ImageNumberH = 5;

		int WantedWidth = 256;
		int WantedHeight = 204;

		int BitmapWidth  = FreeImage_GetWidth(bitmap) / ImageNumberW;
		int BitmapHeight = FreeImage_GetHeight(bitmap) / ImageNumberH;

		int OffsetX = 0;
		int OffsetY = 0;

		// split the image
		for (int i = 0; i < ImageNumberH; ++i)
		{
			for (int j = 0; j < ImageNumberW; ++j)
			{
				int left   = j * BitmapWidth;
				int top    = i * BitmapHeight;

				left += (BitmapWidth - WantedWidth) / 2;
				top += (BitmapHeight - WantedHeight) / 2;

				left += OffsetX;
				top  += OffsetY;

				int right  = left + WantedWidth;
				int bottom = top + WantedHeight;

				FIBITMAP* split_image = FreeImage_CreateView(bitmap, left, top, right, bottom);
				split_images.push_back(split_image);
			}
		}
		// reconstitute the image
		FIBITMAP* new_bitmap = FreeImage_Allocate(WantedWidth * ImageNumberW, WantedHeight * ImageNumberH, 32);
		if (new_bitmap != nullptr)
		{
			for (int i = 0; i < ImageNumberH; ++i)
			{
				for (int j = 0; j < ImageNumberW; ++j)
				{
					FIBITMAP * split_image = split_images[(size_t)(i * ImageNumberW + j)];
					if (split_image == nullptr)
						continue;

					int left = j * WantedWidth;
					int top  = i * WantedHeight;
					int alpha = 0;

					FreeImage_Paste(new_bitmap, split_image, left, top, alpha);
				}
			}

			chaos::WinTools::CopyBitmapToClipboard(new_bitmap);

			//char const* new_filename = nullptr;

			//FreeImage_Save(FREE_IMAGE_FORMAT::FIF_PNG, new_bitmap, new_filename);

			FreeImage_Unload(new_bitmap);
		}

		for (FIBITMAP* image : split_images)
			FreeImage_Unload(image);
	}

	virtual BOOL OnDragFile(char const* filename, POINT const& pt) override
	{
		FIBITMAP* bitmap = chaos::ImageTools::LoadImageFromFile(filename);

		//FIBITMAP* bitmap = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_UNKNOWN, filename, 0);
		if (bitmap != nullptr)
		{
			WorkWithImage(filename, bitmap);
			FreeImage_Unload(bitmap);
		}
		return TRUE;
	}

	virtual LRESULT OnWindowSize(int width, int height) override
	{
		InvalidateRect(GetHwnd(), nullptr, true);
		return SimpleWin32Window::OnWindowSize(width, height);
	}


	virtual LRESULT OnEraseBackground(HDC hDC) override
	{
		return 1;
	}

};


class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		chaos::SimpleWin32CreateParam create_params;
		create_params.x         = 10;
		create_params.y         = 10;
		create_params.nWidth    = 300;
		create_params.nHeight   = 300;
		create_params.dwExStyle = 0;
		//  create_params.dwStyle   = WS_POPUP;

		chaos::SimpleWin32Class<MyWindow> c("class1");
		c.CreateWindowHinstance(NULL, "my_window", create_params);
		chaos::SimpleWin32Window::SimpleMessageLoop();		return true;
	}
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


