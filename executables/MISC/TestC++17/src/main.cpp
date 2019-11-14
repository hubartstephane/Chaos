#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>

#include <chaos/ImageTools.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/GLMTools.h>
#include <chaos/MathTools.h>

class MyApplication : public chaos::Application
{
protected:

	static int GetLayerCount(glm::ivec2 const & cell_count)
	{
		assert(cell_count.x > 0);
		assert(cell_count.y > 0);

		int m = std::min(cell_count.x, cell_count.y);
		return (m / 2) + (m & 1);
	}





	static int GetCellLayer(glm::ivec2 const & cell, glm::ivec2 const& cell_count)
	{
		// which border is the cell the more nearby
		return std::min(
			std::min(cell.x, cell_count.x - cell.x - 1),
			std::min(cell.y, cell_count.y - cell.y - 1)
		);
	}

	static float GetPixelAlpha(glm::vec2 const & pos, glm::vec2 const & image_size, glm::ivec2 const& cell_count, float ratio)
	{
		assert(cell_count.x > 0 && cell_count.y > 0);

		// size of a cell
		glm::vec2 cell_size = image_size / chaos::GLMTools::RecastVector<glm::vec2>(cell_count);
		// the cell containing the point considered
		glm::ivec2 cell = chaos::GLMTools::RecastVector<glm::ivec2>(pos / cell_size);


		int layer_count = GetLayerCount(cell_count);
		int cell_layer  = GetCellLayer(cell, cell_count);

		return (float)cell_layer / (float)layer_count;

		//return cell.x / (float)cell_count.x;

	//	float X = chaos::MathTools::CastAndDiv<float>(p.x, image_size.x);
	//	float Y = chaos::MathTools::CastAndDiv<float>(p.y, image_size.y);

	//	return ratio * X * Y;











		return 0.0f;
	}

#if 0

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




#endif


	void GenerateTexture(int file_index, glm::ivec2 const & image_size, glm::ivec2 const& cell_count, float ratio, boost::filesystem::path const & dst_directory_path)
	{
		FIBITMAP* img = chaos::ImageTools::GenFreeImage<chaos::PixelBGRA>(image_size.x, image_size.y, [image_size, cell_count, ratio](chaos::ImageDescription & desc) {
					
			for (int j = 0; j < desc.height; ++j)
			{
				chaos::PixelBGRA * bgra = chaos::ImageTools::GetPixelAddress<chaos::PixelBGRA>(desc, 0, j);
				
				for (int i = 0; i < desc.width; ++i)
				{
					float alpha = GetPixelAlpha(glm::vec2((float)i, (float)j), glm::vec2((float)image_size.x, (float)image_size.y), cell_count, ratio);

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
			GenerateTexture(i, glm::ivec2(160, 90), glm::ivec2(16, 9), ratio, dst_directory_path);
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


