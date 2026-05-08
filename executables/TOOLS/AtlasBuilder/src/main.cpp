#include "chaos/Chaos.h"

#if 0

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
};


class MyApplication : public chaos::Application
{

public:

	MyApplication()
	{
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

#endif


class ImageGenerationParams
{
public:

	bool IsCountValid() const
	{
		return (count > 0 && count < 500);
	}

	bool IsMinSizeXValid() const
	{
		return IsSizeValid(min_size.x) && (min_size.x < max_size.x);
	}

	bool IsMinSizeYValid() const
	{
		return IsSizeValid(min_size.y) && (min_size.y < max_size.y);
	}

	bool IsMaxSizeXValid() const
	{
		return IsSizeValid(max_size.x) && (min_size.x < max_size.x);
	}

	bool IsMaxSizeYValid() const
	{
		return IsSizeValid(max_size.y) && (min_size.y < max_size.y);
	}

	bool IsSizeValid(int size) const
	{
		return (size > 0) && (size < 1024);
	}

public:

	int count = 15;
	glm::ivec2 min_size = { 1, 1 };
	glm::ivec2 max_size = { 256, 256 };
};

class AtlasBuilderWindow : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(AtlasBuilderWindow, chaos::Window);

public:

	virtual void OnDrawImGuiContent() override
	{
		chaos::ImGuiTools::FullViewportWindow("content", 0, [this]()
		{
			ImGui::SeparatorText("Generate images");

			//ImGui::InputInt("count", &image_generation_params.count);
			//ImGui::InputInt2("min size", &image_generation_params.min_size.x, image_generation_params.min_size.y);
			//ImGui::InputInt2("max size", &image_generation_params.max_size.x, image_generation_params.max_size.y);

			if (ImGui::BeginTable("image_generation_params", 2, 0))
			{
				// count 
				bool count_valid = image_generation_params.IsCountValid();

				ImGui::TableNextColumn();
				ImGui::Text("count");

				ImGui::TableNextColumn();
				SelectTextColor(count_valid, [&]()
				{
					ImGui::InputInt("##count", &image_generation_params.count);
				});

				// min size
				bool min_size_x_valid = image_generation_params.IsMinSizeXValid();
				bool min_size_y_valid = image_generation_params.IsMinSizeYValid();

				ImGui::TableNextColumn();
				ImGui::Text("min size");

				ImGui::TableNextColumn();
				SelectTextColor(min_size_x_valid, [&]()
				{
					ImGui::InputInt("##min_size.x", &image_generation_params.min_size.x);
				});

				ImGui::TableNextColumn();
				//ImGui::Text("##min size.y.title");

				ImGui::TableNextColumn();
				SelectTextColor(min_size_y_valid, [&]()
				{
					ImGui::InputInt("##min_size.y", &image_generation_params.min_size.y);
				});

				// max size
				bool max_size_x_valid = image_generation_params.IsMaxSizeXValid();
				bool max_size_y_valid = image_generation_params.IsMaxSizeYValid();

				ImGui::TableNextColumn();
				ImGui::Text("max size");

				ImGui::TableNextColumn();
				SelectTextColor(max_size_x_valid, [&]()
				{
					ImGui::InputInt("##max_size.x", &image_generation_params.max_size.x);
				});

				ImGui::TableNextColumn();
				//ImGui::Text("##max size.y.title");

				ImGui::TableNextColumn();
				SelectTextColor(max_size_y_valid, [&]()
				{
					ImGui::InputInt("##max_size.y", &image_generation_params.max_size.y);
				});




				

				ImGui::EndTable();
			};

			ImGui::SeparatorText("Generate altas");
		
		});
	}

	virtual void OnDropFile(int count, char const** paths) override
	{



	}

protected:

	void SelectTextColor(bool normal, chaos::LightweightFunction<void()> func)
	{
		ImVec4 normal_color = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImVec4 error_color  = ImVec4(1.0f, 0, 0, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Text, normal ? normal_color : error_color);
		func();
		ImGui::PopStyleColor(1);
	}

protected:

	ImageGenerationParams image_generation_params;

};


class AtlasBuilderApplication : public chaos::SimpleWindowApplication
{
	using chaos::SimpleWindowApplication::SimpleWindowApplication;

	CHAOS_DECLARE_OBJECT_CLASS(AtlasBuilderApplication, chaos::SimpleWindowApplication);

protected:

	virtual int MainBody() override
	{
		//chaos::WinTools::ShowFile(GetResourcesPath());
		return chaos::SimpleWindowApplication::MainBody();
	}

};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<AtlasBuilderApplication>(argc, argv, env, AtlasBuilderWindow::GetStaticClass());
}