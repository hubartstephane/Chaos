#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>

#include <chaos/ImageTools.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/MathTools.h>

class MyApplication : public chaos::Application
{
protected:

	static float GetPixelAlpha(glm::ivec2 const & p, glm::ivec2 const & image_size, glm::ivec2 const& cell_size, float ratio)
	{
		float X = chaos::MathTools::CastAndDiv<float>(p.x, image_size.x);
		float Y = chaos::MathTools::CastAndDiv<float>(p.y, image_size.y);

		return ratio * X * Y;











		return 0.0f;
	}

	static float GetPixelAlpha(glm::ivec2 const& p, int w0, int h0)
	{
		assert(p.x >= 0 && p.x <= w0 - 1);
		assert(p.y >= 0 && p.y <= h0 - 1);


		int layer_index = std::min(
			std::min(p.x, w0 - p.x - 1),
			std::min(p.y, h0 - p.y - 1));




		return 0.0f;
	}

	static int GetDimension(int s0, int layer_index)
	{
		return s0 - 2 * layer_index;
	}


	static int GetElementPerLayer(int w0, int h0, int layer_index)
	{
		assert(w0 > 0);
		assert(h0 > 0);

		int wn = GetDimension(w0, layer_index);
		int hn = GetDimension(h0, layer_index);

		if (w0 == 1)
			return (h0 / 2) + (h0 & 1); 
		else if (h0 == 1)
			return (w0 / 2) + (w0 & 1);
		else
			return w0 + h0 - 2 - 4 * layer_index;
	}

	static int GetLayerCount(int w0, int h0)
	{
		assert(w0 > 0);
		assert(h0 > 0);

		int s0 = std::min(w0, h0);
		return (s0 / 2) + (s0 & 1);
	}





	void GenerateTexture(int file_index, glm::ivec2 const & image_size, glm::ivec2 const& cell_size, float ratio, boost::filesystem::path const & dst_directory_path)
	{
		FIBITMAP* img = chaos::ImageTools::GenFreeImage<chaos::PixelBGRA>(image_size.x, image_size.y, [image_size, cell_size, ratio](chaos::ImageDescription & desc) {
					
			for (int j = 0; j < desc.height; ++j)
			{
				chaos::PixelBGRA * bgra = chaos::ImageTools::GetPixelAddress<chaos::PixelBGRA>(desc, 0, j);
				
				for (int i = 0; i < desc.width; ++i)
				{
					float alpha = GetPixelAlpha(glm::ivec2(i, j), image_size, cell_size, ratio);

					bgra[i].R = (char)(alpha * 255.0f);
					bgra[i].G = (char)(alpha * 255.0f);
					bgra[i].B = (char)(alpha * 255.0f);
					bgra[i].A = 255;
				}
			}
		});

		if (img == nullptr)
			return;

		std::string format = chaos::StringTools::Printf("generated_%d.png", file_index);

		boost::filesystem::path dst_path = (dst_directory_path / boost::filesystem::path(format));

		if (!FreeImage_Save(FIF_PNG, img, dst_path.string().c_str(), 0))
			return;

		FreeImage_Unload(img);

		if (file_index == 0)
			chaos::WinTools::ShowFile(dst_path.string().c_str());
	}

	virtual bool Main() override
	{
		boost::filesystem::path dst_directory_path = GetUserLocalTempPath() / "GeneratedTexture";
		if (!boost::filesystem::is_directory(dst_directory_path))
			if (!boost::filesystem::create_directories(dst_directory_path))
				return false;

		int texture_count = 20;
		for (int i = 0; i < texture_count; ++i)
		{
			float ratio = chaos::MathTools::CastAndDiv<float>(i, texture_count - 1);
			GenerateTexture(i, glm::ivec2(160, 90), glm::ivec2(10, 10), ratio, dst_directory_path);
		}
			
		chaos::WinTools::ShowFile(dst_directory_path);

		return true;
	}
};

int CHAOS_MAIN(int argc, char** argv, char** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);

	return 0;
}


