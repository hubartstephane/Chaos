#include "chaos/Chaos.h"

class MyWindow : public chaos::SimpleWin32Window
{
public:

	void WorkWithImage(char const* filename, FIBITMAP* bitmap)
	{
		std::vector<FIBITMAP*> split_images;

		int SrcImageNumberW = 2;
		int SrcImageNumberH = 4;

		int SrcBitmapWidth  = FreeImage_GetWidth(bitmap) / SrcImageNumberW;
		int SrcBitmapHeight = FreeImage_GetHeight(bitmap) / SrcImageNumberH;

		int DstImageNumberW = 2;
		int DstImageNumberH = 4;

		int DstFullImageNumberW = 2;
		int DstFullImageNumberH = 4;

		int DstBitmapWidth = 190;
		int DstBitmapHeight = 250;


		int OffsetX = 0;
		int OffsetY = 0;

		// split the image
		for (int i = 0; i < SrcImageNumberH; ++i)
		{
			for (int j = 0; j < SrcImageNumberW; ++j)
			{
				int left   = j * SrcBitmapWidth;
				int top    = i * SrcBitmapHeight;

				left += (SrcBitmapWidth - DstBitmapWidth) / 2;
				top += (SrcBitmapHeight - DstBitmapHeight) / 2;

				left += OffsetX;
				top  += OffsetY;

				int right  = left + DstBitmapWidth;
				int bottom = top + DstBitmapHeight;

				FIBITMAP* split_image = FreeImage_Copy(bitmap, left, top, right, bottom);

				chaos::ImageDescription desc = chaos::ImageTools::GetImageDescription(split_image);

				split_images.push_back(split_image);
			}
		}
		// reconstitute the image
		FIBITMAP* new_bitmap = FreeImage_Allocate(DstBitmapWidth * DstFullImageNumberW, DstBitmapHeight * DstFullImageNumberH, 32);
		if (new_bitmap != nullptr)
		{
			RGBQUAD background = { 0, 0, 0, 0 };
			FreeImage_SetBackgroundColor(new_bitmap, &background);
			FreeImage_FillBackground(new_bitmap, &background);

			for (size_t i = 0; i < split_images.size(); ++i)
			{
				int PosX = ((int)i) % DstImageNumberW;
				int PosY = ((int)i) / DstImageNumberW;

				FIBITMAP* split_image = split_images[i];
				if (split_image == nullptr)
					continue;

				int left = PosX * DstBitmapWidth;
				int top = PosY * DstBitmapHeight;
				int alpha = 255;
				FreeImage_Paste(new_bitmap, split_image, left, top, alpha);

			}

			chaos::Application* application = chaos::Application::GetInstance();
			if (application != nullptr)
			{
				boost::filesystem::path const& user_path = application->GetUserLocalTempPath();

				boost::filesystem::path dest = user_path / boost::filesystem::path(filename).filename();

				FreeImage_Save(FREE_IMAGE_FORMAT::FIF_PNG, new_bitmap, dest.string().c_str());
			}

			FreeImage_Unload(new_bitmap);
		}

		for (FIBITMAP* image : split_images)
			FreeImage_Unload(image);
	}

	virtual BOOL OnDragFile(char const* filename, POINT const& pt) override
	{
		FIBITMAP* bitmap = chaos::ImageTools::LoadImageFromFile(filename);
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

public:

	MyApplication()
	{
		show_console = false;
	}

protected:

	virtual int Main() override
	{
		boost::filesystem::path user_temp = CreateUserLocalTempDirectory();
		chaos::WinTools::ShowFile(user_temp);


		chaos::SimpleWin32CreateParam create_params;
		create_params.x         = 10;
		create_params.y         = 10;
		create_params.nWidth    = 300;
		create_params.nHeight   = 300;
		create_params.dwExStyle = 0;
		//  create_params.dwStyle   = WS_POPUP;

		chaos::SimpleWin32Class<MyWindow> c("class1");
		c.CreateWindowHinstance(NULL, "my_window", create_params);
		chaos::SimpleWin32Window::SimpleMessageLoop();
		
		return 0;
	}
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}