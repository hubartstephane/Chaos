

#include "chaos/Chaos.h"

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
RecastVectorData<T> RecastVector(T const& in_value)
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
	static int GetLayerCount(glm::ivec2 const& grid_size)
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
	static int GetCategoryCountOnLayer(glm::ivec2 const& grid_size, int layer_index)
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
	static int GetCellLayer(glm::ivec2 const& cell, glm::ivec2 const& grid_size)
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





	static glm::vec4 GetPixelColorForCell(glm::vec2 const& cell, glm::vec2 const& grid_size, float ratio)
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

		static bool init = false;

		int constexpr color_count = 4;

		static glm::vec4 values[color_count];
		if (!init)
		{
			init = true;
			for (int i = 0; i < color_count; ++i)
				values[i] = chaos::GLMTools::RandVec4();

			values[0] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			values[1] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			values[2] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			values[3] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);


		}

		int index = (int)(cell.x + cell.y * grid_size.x);

		//	return values[index % color_count];

		assert(a1 >= 0.0f && a1 <= 1.0f);
		assert(a2 >= 0.0f && a2 <= 1.0f);

		float result = 0.0f;
		if (ratio <= a1)
			result = 1.0f;
		else if (ratio >= a2)
			result = 0.0f;
		else
			result = 1.0f - ((ratio - a1) / (a2 - a1));


		return glm::vec4(result, result, result, 1.0f);
	}


	static glm::vec4 GetPixelColor(glm::vec2 const& pos, glm::vec2 const& image_size, glm::ivec2 const& grid_size, float ratio, int recurse_count)
	{
		assert(grid_size.x > 0 && grid_size.y > 0);

		// size of a cell
		glm::vec2 cell_size = image_size / chaos::RecastVector<glm::vec2>(grid_size);
		// the cell containing the point considered
		glm::ivec2 cell = chaos::RecastVector<glm::vec2>(pos / cell_size);

		// 9 adjacent cells -> 9 values considered -> 9 distances with the center of the cells
		//
		// the value of the point is an 'average' of all the 9 values
		// => the difficulty for this is that whenever a point is exactly at the center, the distance is the smallest and the influence must be the greatest (unlike normal average)
		//
		// => solution with an example (for a 4 cells)
		//
		// VALUE =
		//   v1.(d2.d3.d4)/SUM +
		//   V2.(d1.d3.d4)/SUM +   where SUM = (d2.d3.d4) + (d1.d3.d4) + (d1.d2.d3) + (d2.d3.d4)
		//   V3.(d1.d2.d4)/SUM +
		//   V4.(d1.d2.d3)/SUM
		//
		// With such a solution, whenever on distance is 0, all other factors are zeroed
		// => the only cell value considered is the one for which the distance is 0
		// => the sum of all coefficients are 0
		//
		// XXX : in fact, the only distance that may be 0, is the one of the central grid (maybe some opimization to do later)

		std::array<float, 9> distances = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f };		// -1.0f for invalid values
		std::array<glm::vec4, 9> values;

		float distance_product = 1.0f; // utility value to simplify the computation of SUM

		for (int j = -1; j <= 1; ++j)
		{
			for (int i = -1; i <= 1; ++i)
			{
				// test whether the neighboor cell is still in the grid
				glm::ivec2 neighboor_cell = cell;
				neighboor_cell.x += i;
				neighboor_cell.y += j;
				if (neighboor_cell.x < 0 || neighboor_cell.y < 0)
					continue;
				if (neighboor_cell.x >= grid_size.x || neighboor_cell.y >= grid_size.y)
					continue;

				//if (i != 0 || j != 0)
				//		continue;

					// compute value of that neighboor
				int index = 1 + i + (j + 1) * 3;
				glm::vec4 neighboor_value = GetPixelColorForCell(neighboor_cell, grid_size, ratio);

				// compute distance of that neighboor => distance = 0 bring early exit
				glm::vec2 neighboor_center = cell_size * chaos::RecastVector<glm::vec2>(neighboor_cell) + cell_size * 0.5f;

				float d = glm::distance(pos, neighboor_center);
				if (d == 0.0f)
					return neighboor_value; // early exit

#if 0
				if (pos.x >= neighboor_cell.x && i < 0)
					continue;
				if (pos.x <= neighboor_cell.x && i > 0)
					continue;
				if (pos.y >= neighboor_cell.y && j < 0)
					continue;
				if (pos.y <= neighboor_cell.y && j > 0)
					continue;
#endif




				// store the values
				distances[index] = d;
				values[index] = neighboor_value;
				distance_product *= d;
			}
		}

		// computation of SUM
		float sum = 0.0f;
		for (int i = 0; i < 9; ++i)
			if (distances[i] > 0.0f) // should never be 0
				sum += distance_product / distances[i]; // (d2.d3.d4) = (d1.d2.d3.d4)/d1

		// computation of the 'average'
		glm::vec4 result = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 9; ++i)
		{
			float d = distances[i];
			if (d <= 0.0f)
				continue;
			result += values[i] * (distance_product / distances[i]) / sum;
		}
		return result;
	}




	static glm::vec4 GetPixelColor2(glm::vec2 const& pos, glm::vec2 const& image_size, glm::ivec2 const& grid_size, float ratio, int recurse_count)
	{
		assert(image_size.x > 0);
		assert(image_size.y > 0);

		// compute the diagonale of the rectangle and the angle beta to the X Axis
		glm::vec2 diag = image_size * 0.5f;
		float     diag_length = glm::length(diag);

		float beta = std::atan2(diag.y, diag.x);

		// considering the ellipse contained by the rectangle
		// it is described by
		//
		// | cos(t). W/2
		// |
		// | sin(t). H/2
		//
		// for t = beta, its length is
		glm::vec2 d = diag * glm::vec2(std::cos(beta), std::sin(beta));

		float d_length = glm::length(d);

		// the factor to apply to the ellipse so it fully contains the rectangle is
		float scale_factor = diag_length / d_length;

		// the radius = (Rx, Ry) is (start of the blend-out zone)
		glm::vec2 min_radius = scale_factor * diag;

		// the radius for the end of the blend-out zone is (+10% radius)
		glm::vec2 max_radius = scale_factor * (diag * 1.25f);

		// compute for the considered point the angle with X axis
		glm::vec2 center = image_size * 0.5f;

		glm::vec2 dp = pos - center;

		float alpha = (dp == glm::vec2(0.0f, 0.0f)) ? 0.0f : std::atan2(dp.y, dp.x);

		// compute the distant point on the ellipse
		glm::vec2 a = glm::vec2(std::cos(alpha), std::sin(alpha));

		glm::vec2 min_limit = min_radius * a;
		glm::vec2 max_limit = max_radius * a;

		// compute the distance from center to the 2 limits
		float min_distance = glm::length(min_limit);
		float max_distance = glm::length(max_limit);
		float p_distance = glm::length(dp);

		p_distance += ratio * max_distance;

		float value = 0.0f;

		if (p_distance <= min_distance)
			value = 1.0f;
		else if (p_distance >= max_distance)
			value = 0.0f;
		else
			value = 1.0f - (p_distance - min_distance) / (max_distance - min_distance);

		return glm::vec4(value, value, value, value);
	}



	void GenerateTexture(int file_index, glm::ivec2 const& image_size, glm::ivec2 const& cell_count, float ratio, int recurse_count, boost::filesystem::path const& dst_directory_path)
	{
		FIBITMAP* img = chaos::ImageTools::GenFreeImage<chaos::PixelBGRA>(image_size.x, image_size.y, [image_size, cell_count, ratio, recurse_count](chaos::ImageDescription& desc) {

			for (int j = 0; j < desc.height; ++j)
			{
				chaos::ImagePixelAccessor<chaos::PixelBGRA> accessor(desc);

				chaos::PixelBGRA* line = &accessor(0, j);

				for (int i = 0; i < desc.width; ++i)
				{


					glm::vec4 color = GetPixelColor2(glm::vec2((float)i, (float)j), glm::vec2((float)image_size.x, (float)image_size.y), cell_count, ratio, recurse_count);



					line[i].R = (char)(color.x * 255.0f);
					line[i].G = (char)(color.y * 255.0f);
					line[i].B = (char)(color.z * 255.0f);
					line[i].A = 255;
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

	virtual int Main() override
	{
		boost::filesystem::path dst_directory_path = GetUserLocalTempPath() / "GeneratedTexture";
		if (!boost::filesystem::is_directory(dst_directory_path))
			if (!boost::filesystem::create_directories(dst_directory_path))
				return -1;

		glm::ivec2 image_size = glm::ivec2(320, 200);
		glm::ivec2 cell_count = glm::ivec2(8, 5);

		int texture_count = 160;
		int recurse_count = 1;

		for (int i = 0; i < texture_count; ++i)
		{
			float ratio = chaos::MathTools::CastAndDiv<float>(i, texture_count - 1);
			GenerateTexture(i, image_size, cell_count, ratio, recurse_count, dst_directory_path);
		}

		chaos::WinTools::ShowFile(dst_directory_path);

		return 0;
	}
};

int main(int argc, char** argv, char** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}