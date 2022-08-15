#include <chaos/Chaos.h>

class MyApplication : public chaos::Application
{
protected:


	FIBITMAP* GenerateTexture(glm::ivec2 const& image_size)
	{
		return chaos::ImageTools::GenFreeImage<chaos::PixelBGRA>(image_size.x, image_size.y, [image_size](chaos::ImageDescription& desc)
		{
			chaos::ImagePixelAccessor<chaos::PixelBGRA> accessor(desc);

			for (int j = 0; j < desc.height; ++j)
			{
				chaos::PixelBGRA* line = &accessor(0, j);

				for (int i = 0; i < desc.width; ++i)
				{
					float x = ((float)i) / ((float)image_size.x);
					float y = ((float)j) / ((float)image_size.y);

					float max_alpha = 1.0f;
					float distance_x = 0.2f;
					float distance_y = 0.2f;

					float alpha_x = 1.0f;
					float alpha_y = 1.0f;

					if (x < distance_x)
						alpha_x = 1.0f - ((distance_x - x) / distance_x);
					else if (x > 1.0f - distance_x)
						alpha_x = 1.0f - ((x - (1.0f - distance_x)) / distance_x);

					if (y < distance_y)
						alpha_y = 1.0f - ((distance_y - y) / distance_y);
					else if (y > 1.0f - distance_y)
						alpha_y = 1.0f - ((y - (1.0f - distance_y)) / distance_y);

					float alpha = max_alpha * std::min(alpha_x, alpha_y);

					line[i].R = 255;
					line[i].G = 255;
					line[i].B = 255;
					line[i].A = (char)(255.0f * alpha);
				}
			}



		});
	}

	virtual bool Main() override
	{
		boost::filesystem::path dst_directory_path = GetUserLocalTempPath() / "GeneratedTexture";
		if (!boost::filesystem::is_directory(dst_directory_path))
			if (!boost::filesystem::create_directories(dst_directory_path))
				return false;

		glm::ivec2 image_size = glm::ivec2(128, 128);
		FIBITMAP* image = GenerateTexture(image_size);
		if (image != nullptr)
		{
			boost::filesystem::path dst = dst_directory_path / "TX_TooltipBackground.png";
			FreeImage_Save(FIF_PNG, image, dst.string().c_str(), 0);
			FreeImage_Unload(image);
			chaos::WinTools::ShowFile(dst_directory_path);
		}

		return true;
	}
};

int main(int argc, char** argv, char** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}
