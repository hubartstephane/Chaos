#include <chaos/Chaos.h>

#if 0
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

class WindowOpenGLTest : public chaos::Window
{
public:

	void TestFile(char const * filename)
	{
		chaos::WindowApplication * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return;

		boost::filesystem::path resources_path = application->GetResourcesPath();
		boost::filesystem::path filepath = resources_path / "images" / filename;

#if 0

		cv::InputArray buf = cv::InputArray(nullptr, 0);
		cv::imdecode(buf, 0);

		cv::Mat image = cv::imread(filepath.string().c_str(), cv::IMREAD_COLOR);

		std::vector<cv::Mat> animated;
		imreadmulti(filepath.string().c_str(), animated, cv::IMREAD_COLOR);

		filename = filename;

#endif

		chaos::Buffer<char> * buffer = new chaos::Buffer<char>;
		{
			chaos::Buffer<char> b = chaos::FileTools::LoadFile(filepath, false);
			*buffer = b;
		}

		int cc = FreeImage_GetFIFCount();

		FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(filepath.string().c_str());

		FIBITMAP * bitmap = FreeImage_Load(format, filepath.string().c_str(), 0);
		if (bitmap != nullptr)
			FreeImage_Unload(bitmap);

		FIMEMORY * memory = FreeImage_OpenMemory((BYTE*)buffer->data, (DWORD)buffer->bufsize);


		bool create_new = false;
		bool read_only  = true;
		bool keep_cache_in_memory = true;
	//	FIMULTIBITMAP * multi_bitmap = FreeImage_OpenMultiBitmap(format, filepath.string().c_str(), create_new, read_only, keep_cache_in_memory, 0);


		FIMULTIBITMAP * multi_bitmap = FreeImage_LoadMultiBitmapFromMemory(format, memory, 0);

		//delete buffer;
		//FreeImage_CloseMemory(memory);

		if (multi_bitmap != nullptr)
		{
			int page_count = FreeImage_GetPageCount(multi_bitmap);
			if (page_count > 0)
			{
				FIBITMAP * page = FreeImage_LockPage(multi_bitmap, 0);

				

				if (page != nullptr)
				{
					int bpp = FreeImage_GetBPP(page);
					int w   = FreeImage_GetWidth(page);
					int h = FreeImage_GetHeight(page);

					
					//multi_bitmap = nullptr;

					//
					FreeImage_UnlockPage(multi_bitmap, page, false);
				}


			}



			FreeImage_CloseMultiBitmap(multi_bitmap, 0);
		}


		FreeImage_CloseMemory(memory);

	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!chaos::Window::InitializeFromConfiguration(config, config_path))
			return false;

		TestFile("static_gif.gif");
		TestFile("animated_gif.gif");

		TestFile("static_png.png");
		TestFile("animated_png.png");


		return true;
	}
};



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}


