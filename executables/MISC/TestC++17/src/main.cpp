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

	// The image is split per layers
	// The elements on those layers fade in order (A first, C last)
	//
	// A A A A A A
	// A B B B B A
	// A B C C B A
	// A B B B B A
	// A A A A A A
	// 	
	// for a given the layer the element fade according to their category
	// this create a smooth animation
	//
	// 1 2 3 4 5 6 7
	// 9           8
	// 8           9
	// 7 6 5 4 3 2 1
	//

	// the number of layers
	static int GetLayerCount(glm::ivec2 const & cell_count)
	{
		assert(cell_count.x > 0);
		assert(cell_count.y > 0);

		int m = std::min(cell_count.x, cell_count.y);
		return (m / 2) + (m & 1);
	}

	// the cell count for a given sub layer
	static glm::ivec2 GetLayerCellCount(glm::ivec2 const& cell_count, int layer_index)
	{
		assert(cell_count.x > 0);
		assert(cell_count.y > 0);

		return glm::ivec2(
			cell_count.x - 2 * layer_index,
			cell_count.y - 2 * layer_index);
	}


	// the number of cell categories in a layer
	static int GetCellCategoryOnLayer(glm::ivec2 const & cell_count, int layer_index)
	{
		assert(cell_count.x > 0);
		assert(cell_count.y > 0);

		glm::ivec2 layer_cell_count = GetLayerCellCount(cell_count, layer_index);

		if (layer_cell_count.x == 1)
			return (layer_cell_count.y / 2) + (layer_cell_count.y & 1);
		else if (layer_cell_count.y == 1)
			return (layer_cell_count.x / 2) + (layer_cell_count.x & 1);
		else
			return layer_cell_count.x + layer_cell_count.y - 2;
	}

	// the sum of all categories on all previous layers
	static int GetSumCellCategoryForPreviousLayers(glm::ivec2 const& cell_count, int layer_index)
	{
		if (layer_index == 0)
			return 0;

		int base_category = cell_count.x + cell_count.y - 2; // number of categories for layer 0

		return layer_index * base_category - 4 * (layer_index - 1) * (layer_index) / 2;
	}
	// the layer the cell is on
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

		// the total number of layers
		int layer_count = GetLayerCount(cell_count);
		// the current layer for given cell
		int cell_layer  = GetCellLayer(cell, cell_count);
		// number of categories for that layer
		int category_count = GetCellCategoryOnLayer(cell_count, cell_layer);
		// number of categories in all previous layers
		int previous_category_count = GetSumCellCategoryForPreviousLayers(cell_count, cell_layer);








	//	return (float)category_count / (float)(cell_count.x * cell_count.y);

	//	return (float)cell_layer / (float)layer_count;


//		return (float)cell_layer / (float)layer_count;

		//return cell.x / (float)cell_count.x;

	//	float X = chaos::MathTools::CastAndDiv<float>(p.x, image_size.x);
	//	float Y = chaos::MathTools::CastAndDiv<float>(p.y, image_size.y);

	//	return ratio * X * Y;






#if 0
		if (cell_layer == 0)
			cell_layer = cell_layer;
		if (cell_layer == 1)
			cell_layer = cell_layer;
		if (cell_layer == 2)
			cell_layer = cell_layer;
#endif 




		return 0.0f;
	}

#if 0





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

		glm::ivec2 image_size = glm::ivec2(16, 9);
		glm::ivec2 cell_count = glm::ivec2(16, 9);

		int texture_count = 1;
		for (int i = 0; i < texture_count; ++i)
		{
			float ratio = chaos::MathTools::CastAndDiv<float>(i, texture_count - 1);
			GenerateTexture(i, image_size, cell_count, ratio, dst_directory_path);
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


