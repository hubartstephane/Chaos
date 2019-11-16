#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>

#include <chaos/ImageTools.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>
#include <chaos/GeometryFramework.h>
#include <chaos/MathTools.h>



template<typename VECTOR_TYPE>
class RecastVectorData
{
public:

	RecastVectorData(VECTOR_TYPE const& in_value) : value(in_value) {}

	template<typename DST_VECTOR_TYPE>
	operator DST_VECTOR_TYPE () const
	{
		DST_VECTOR_TYPE result(0);
		size_t count = std::min(result.length(), value.length());
		for (size_t i = 0; i < count; ++i)
			result[i] = static_cast<typename DST_VECTOR_TYPE::value_type>(value[i]);
		return result;
	}

public:

	VECTOR_TYPE value;
};

template<typename T>
RecastVectorData<T> RecastVector(T const & in_value)
{
	return RecastVectorData<T>(in_value);
}















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
	static int GetLayerCount(glm::ivec2 const & grid_size)
	{
		assert(grid_size.x > 0);
		assert(grid_size.y > 0);

		int m = std::min(grid_size.x, grid_size.y);
		return (m / 2) + (m & 1);
	}

	// the grid size for a given sub layer
	static glm::ivec2 GetLayerGridSize(glm::ivec2 const& grid_size, int layer_index)
	{
		assert(grid_size.x > 0);
		assert(grid_size.y > 0);

		return glm::ivec2(
			grid_size.x - 2 * layer_index,
			grid_size.y - 2 * layer_index);
	}


	// the number of cell categories in a layer
	static int GetCategoryCountOnLayer(glm::ivec2 const & grid_size, int layer_index)
	{
		assert(grid_size.x > 0);
		assert(grid_size.y > 0);

		glm::ivec2 layer_grid_size = GetLayerGridSize(grid_size, layer_index);

		if (layer_grid_size.x == 1)
			return (layer_grid_size.y / 2) + (layer_grid_size.y & 1);
		else if (layer_grid_size.y == 1)
			return (layer_grid_size.x / 2) + (layer_grid_size.x & 1);
		else
			return layer_grid_size.x + layer_grid_size.y - 2;
	}

	// the sum of all categories on all previous layers
	static int GetSumCategoryForPreviousLayers(glm::ivec2 const& grid_size, int layer_index)
	{
		if (layer_index == 0)
			return 0;

		int base_category = grid_size.x + grid_size.y - 2; // number of categories for layer 0

		return layer_index * base_category - 4 * (layer_index - 1) * (layer_index) / 2;
	}
	// the layer the cell is on
	static int GetCellLayer(glm::ivec2 const & cell, glm::ivec2 const& grid_size)
	{
		// which border is the cell the more nearby
		return std::min(
			std::min(cell.x, grid_size.x - cell.x - 1),
			std::min(cell.y, grid_size.y - cell.y - 1)
		);
	}

	static int GetCellCategory(glm::ivec2 const& cell, glm::ivec2 const& grid_size, int layer_index)
	{
		// get the grid size for required layer
		glm::ivec2 layer_grid_size = GetLayerGridSize(grid_size, layer_index);
		// move the cell to the origin of the considered layer grid
		glm::ivec2 c = cell - glm::ivec2(layer_index, layer_index);
		// degenerated cases for WIDTH = 0
		if (layer_grid_size.x == 1)
		{
			if (c.y < layer_grid_size.y / 2)
				return c.y;
			else
				return layer_grid_size.y - 1 - c.y;
		}
		// degenerated cases for HEIGHT = 0
		else if (layer_grid_size.y == 1)
		{
			if (c.x < layer_grid_size.x / 2)
				return c.x;
			else
				return layer_grid_size.x - 1 - c.x;
		}
		// normal cases (test for each borders of the grid)
		else
		{
			if (c.y == 0)
				return c.x;
			else if (c.y == layer_grid_size.y - 1)
				return layer_grid_size.x - 1 - c.x;
			else if (c.x == 0)
				return layer_grid_size.x + (layer_grid_size.y - 1 - c.y - 1);
			else if (c.x == layer_grid_size.x - 1)
				return layer_grid_size.x + c.y - 1;
		}
		return 0;
	}


	static float GetPixelAlphaForCell(glm::vec2 const& cell, glm::vec2 const& grid_size, float ratio)
	{
		// the total number of layers
		int layer_count = GetLayerCount(grid_size);
		// the current layer for given cell
		int cell_layer = GetCellLayer(cell, grid_size);
		// number of categories for that layer
		int category_count = GetCategoryCountOnLayer(grid_size, cell_layer);
		// number of categories in all previous layers
		int sum_previous_category_count = GetSumCategoryForPreviousLayers(grid_size, cell_layer);
		// total number of categories
		int sum_category_count = GetSumCategoryForPreviousLayers(grid_size, layer_count - 1) + GetCategoryCountOnLayer(grid_size, layer_count - 1);
		// category of cell according to its layer
		int cell_category = GetCellCategory(cell, grid_size, cell_layer);
		// final category for layer
		int absolute_cell_category = cell_category + sum_previous_category_count;

		// this cell change its alpha when it is between the following categories
		float a1 = (float)absolute_cell_category / (float)sum_category_count;
		float a2 = a1 + 1.0f / (float)sum_category_count;

		assert(a1 >= 0.0f && a1 <= 1.0f);
		assert(a2 >= 0.0f && a2 <= 1.0f);

		float result = 0.0f;
		if (ratio <= a1)
			result = 1.0f;
		else if (ratio >= a2)
			result = 0.0f;
		else
			result = 1.0f - ((ratio - a1) / (a2 - a1));

		return result;
	}

	static float GetPixelAlpha(glm::vec2 const & pos, glm::vec2 const & image_size, glm::ivec2 const& grid_size, float ratio, int recurse_count)
	{
		assert(grid_size.x > 0 && grid_size.y > 0);

		// size of a cell
		glm::vec2 cell_size = image_size / chaos::RecastVector<glm::vec2>(grid_size);
		// the cell containing the point considered
		glm::ivec2 cell = chaos::RecastVector<glm::vec2>(pos / cell_size);

		
#if 0		
		std::array<float, 9> distances;
		std::array<float, 9> values;

		for (int j = -1; j <= 1; ++j)
		{
			for (int i = -1; i <= 1; ++i)
			{
				glm::ivec2 neighboor_cell = cell;
				neighboor_cell.x += i;
				neighboor_cell.y += j;
				if (neighboor_cell.x < 0 || neighboor_cell.y < 0)
					continue;
				if (neighboor_cell.x >= grid_size.x || neighboor_cell.y >= grid_size.y)
					continue;

				int index = 1 + i + (j + 1) * 3;

				float neighboor_value = GetPixelAlphaForCell(neighboor_cell, grid_size, ratio);

				glm::vec2 neighboor_center = cell_size * chaos::RecastVector<glm::vec2>(neighboor_cell) + cell_size * 0.5f;

				float d = glm::distance(pos, neighboor_center);
				if (d == 0.0f)
					return neighboor_value;




			}
		}
		
		
		
#endif		
		
		
		
		
		
		
		
		
		
		std::array<float, 9> neighboor_factors { 0.25f, 0.5f, 0.25f, 0.5f, 1.0f, 0.5f, 0.25f, 0.5f, 0.25f };

		float cell_value = 0.0f;

		float sum_factor = 0.0f;
		float sum_ratio  = 0.0f;
		for (int j = -1; j <= 1; ++j)
		{
			for (int i = -1; i <= 1; ++i)
			{
				glm::ivec2 neighboor_cell = cell;
				neighboor_cell.x += i;
				neighboor_cell.y += j;
				if (neighboor_cell.x < 0 || neighboor_cell.y < 0)
					continue;
				if (neighboor_cell.x >= grid_size.x || neighboor_cell.y >= grid_size.y)
					continue;

#if 0

				int index = 1 + i + (j + 1) * 3;
				float factor = neighboor_factors[index];
				float value = GetPixelAlphaForCell(neighboor_cell, grid_size, ratio);
				if (i == 0 && j == 0)
					cell_value = value;
				sum_ratio += factor * value;
				sum_factor += factor;


#elif 1
				float value = GetPixelAlphaForCell(neighboor_cell, grid_size, ratio);

				glm::vec2 neighboor_center = cell_size * chaos::RecastVector<glm::vec2>(neighboor_cell) + cell_size * 0.5f;

				float d = glm::distance(pos, neighboor_center);
				if (d == 0.0f)
				{
					sum_ratio = value;
					sum_factor = 1.0f;
					break;
				}
				else
				{

				}


#endif

	//			if (i == 0 && j == 0)
	//				cell_value = value;
	//			sum_ratio += factor * value;
	//			sum_factor += factor;
			}
		}

		float result = sum_ratio / sum_factor;

#if 0

		if (recurse_count-- > 0 && cell_value > 0.0f && cell_value < 1.0f) // 
		{
			int s = std::min(grid_size.x, grid_size.y);

			glm::ivec2 child_grid_size = glm::ivec2(s, s);

			glm::vec2 child_image_size = cell_size;

			glm::vec2 cell_pos = cell_size * glm::vec2((float)cell.x, (float)cell.y);
			glm::vec2 child_pos = pos - cell_pos;

			float child_ratio = 1.0f - cell_value;

			float alpha_child = GetPixelAlpha(child_pos, child_image_size, child_grid_size, child_ratio, recurse_count);

			result = result + alpha_child * (cell_value - result);
		}

#endif

		return result;
	}

	void GenerateTexture(int file_index, glm::ivec2 const & image_size, glm::ivec2 const& cell_count, float ratio, int recurse_count, boost::filesystem::path const & dst_directory_path)
	{
		FIBITMAP* img = chaos::ImageTools::GenFreeImage<chaos::PixelBGRA>(image_size.x, image_size.y, [image_size, cell_count, ratio, recurse_count](chaos::ImageDescription & desc) {
					
			for (int j = 0; j < desc.height; ++j)
			{
				chaos::PixelBGRA * bgra = chaos::ImageTools::GetPixelAddress<chaos::PixelBGRA>(desc, 0, j);
				
				for (int i = 0; i < desc.width; ++i)
				{
					float alpha = GetPixelAlpha(glm::vec2((float)i, (float)j), glm::vec2((float)image_size.x, (float)image_size.y), cell_count, ratio, recurse_count);

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

		glm::ivec2 image_size = glm::ivec2(640, 400);
		glm::ivec2 cell_count = glm::ivec2(8, 5);

		int texture_count = 160;
		int recurse_count = 1;

		for (int i = 0; i < texture_count; ++i)
		{
			float ratio = chaos::MathTools::CastAndDiv<float>(i, texture_count - 1);
			GenerateTexture(i, image_size, cell_count, ratio, recurse_count, dst_directory_path);
		}
			
		chaos::WinTools::ShowFile(dst_directory_path);

		return true;
	}
};

int CHAOS_MAIN(int argc, char** argv, char** env)
{
	glm::vec3 a = { 1.5f, 2.0f , -3.6f };

	glm::ivec4 b = RecastVector(a);

	glm::vec2 c = RecastVector(b);

	chaos::RunApplication<MyApplication>(argc, argv, env);

	return 0;
}


